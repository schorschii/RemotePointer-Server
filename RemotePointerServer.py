#!/usr/bin/env python3

from PyQt6 import QtWidgets, QtGui, QtCore
from locale import getlocale

from functools import partial
from pathlib import Path
from pynput.mouse import Button, Controller as MouseController
from pynput.keyboard import Key, Controller as KeyboardController
import pyperclip
import socket
import psutil
import threading
import time
import configparser
import argparse
import os, sys
import webbrowser
import glob
import secrets

APP_NAME    = 'RemotePointer'
APP_VERSION = '3.1'

if sys.platform == 'win32':
    CONFIG_DIR  = os.path.expanduser('~/AppData/Local')
else:
    CONFIG_DIR  = os.path.expanduser('~/.config')
os.makedirs(CONFIG_DIR, exist_ok=True)
CONFIG_FILE = CONFIG_DIR + '/RemotePointer.ini'

POINTERS_DIR = (os.path.dirname(sys.executable) if getattr(sys, 'frozen', False) else sys.path[0]) + '/res/pointers'
# for packed .app on macOS
if(sys.platform == 'darwin' and not os.path.exists(POINTERS_DIR)):
    POINTERS_DIR = os.path.dirname(sys.executable) + '/../Resources/res/pointers'

DEFAULT_CONFIG = {
    'pointer': {
        'style': '',
        'speed': 13,
    },
    'mouse': {
        'speed': 1,
    },
    'server': {
        'listenPort': 4444,
        'broadcastPort': 4445,
        'code': '',
        'hostname': '',
    }
}


class AdvertiseBroadcast(threading.Thread):
    broadcastPort = 0
    listenPort = 0
    hostname = 0

    def __init__(self, broadcastPort, listenPort, hostname, *args, **kwargs):
        self.broadcastPort = broadcastPort
        self.listenPort = listenPort
        self.hostname = hostname
        # call Thread constructor
        super(AdvertiseBroadcast, self).__init__(*args, **kwargs)
        self.daemon = True

    def run(self, *args, **kwargs):
        while True:
            ifaces = psutil.net_if_addrs()
            for ifacename, iface in ifaces.items():
                for snic in iface:
                    if(snic.family != socket.AF_INET): continue
                    viaAddress = snic.address
                    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) as sock:
                        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
                        try:
                            if(viaAddress != '127.0.0.1'):
                                # sending packet via secondary interfaces
                                sock.bind((viaAddress, 0))
                            #else: # sending packet via default interface
                            sock.sendto(bytes('HELLOREMOTEPOINTER|'+self.hostname, encoding='utf8'), ('255.255.255.255', self.broadcastPort))
                        except Exception as e:
                            print(e)
                            continue
            time.sleep(0.9)

class PointerWindow(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super(PointerWindow, self).__init__(parent)
        self.setStyleSheet('background:transparent;')
        self.setAttribute(QtCore.Qt.WidgetAttribute.WA_TranslucentBackground)
        self.setWindowFlags(
            QtCore.Qt.WindowType.FramelessWindowHint
            | QtCore.Qt.WindowType.WindowStaysOnTopHint
            | QtCore.Qt.WindowType.Tool # no taskbar entry
        )
        self.img = QtWidgets.QLabel()
        self.layout = QtWidgets.QGridLayout(self)
        self.layout.addWidget(self.img, 0, 0)
        self.setLayout(self.layout)

class ControlListener(threading.Thread):
    def __init__(self, listenPort, code, evtPointerShowHide, evtPointerMove, mouseSpeed, *args, **kwargs):
        self.listenPort = listenPort
        self.code = code
        self.evtPointerShowHide = evtPointerShowHide
        self.evtPointerMove = evtPointerMove
        self.mouseSpeed = mouseSpeed
        self.updatePointer = False
        self.mouse = MouseController()
        self.keyboard = KeyboardController()
        self.connection = None
        # call Thread constructor
        super(ControlListener, self).__init__(*args, **kwargs)
        self.daemon = True
        # setup cliboard watcher
        t = threading.Thread(target=self.clipboardWatcher)
        t.daemon = True # exit when main thread exits
        t.start()

    def clipboardWatcher(self):
        self.recentClipboard = ''
        while True:
            try:
                tmpValue = pyperclip.paste()
                if tmpValue != self.recentClipboard:
                    self.recentClipboard = tmpValue
                    if self.connection:
                        self.connection.sendall(('CLIPBOARD|'+tmpValue+'\n').encode('utf-8'))
            except Exception:
                pass
            time.sleep(0.2)

    def run(self, *args, **kwargs):
        while True:
            try:
                self.openSocket()
            except Exception as e:
                print(e)
                time.sleep(0.1)

    def openSocket(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.bind(('0.0.0.0', self.listenPort))
            sock.listen(0)
            connection, address = sock.accept()
            with connection:
                print('Connection from: ' + str(address))
                connection.sendall('HELLO!\n'.encode('utf-8'))
                clientCode = connection.recv(1024).decode()
                if not clientCode or clientCode.strip() != self.code:
                    connection.sendall('AUTHFAILED\n'.encode('utf-8'))
                    print('Invalid code:', clientCode)
                else:
                    self.connection = connection
                    while True:
                        data = connection.recv(1024).decode()
                        if not data: break
                        for d in data.split('\n'):
                            self.handleMessage(d)

    def handleMessage(self, data):
        if data == 'START':
            self.evtPointerShowHide.emit(True)
            self.updatePointer = True
        elif data == 'STOP':
            self.evtPointerShowHide.emit(False)
            self.updatePointer = False

        elif data == 'PREV': self.keyboard.tap(Key.page_up)
        elif data == 'NEXT': self.keyboard.tap(Key.page_down)
        elif data == 'RETURN': self.keyboard.tap(Key.enter)
        elif data == 'BACKSPACE': self.keyboard.tap(Key.backspace)
        elif data == 'ESCAPE': self.keyboard.tap(Key.esc)
        elif data == 'MDOWN': self.mouse.press(Button.left)
        elif data == 'MUP': self.mouse.release(Button.left)
        elif data == 'MLEFT': self.mouse.click(Button.left)
        elif data == 'MRIGHT': self.mouse.click(Button.right)
        elif data == 'UP': self.keyboard.tap(Key.up)
        elif data == 'DOWN': self.keyboard.tap(Key.down)
        elif data == 'LEFT': self.keyboard.tap(Key.left)
        elif data == 'RIGHT': self.keyboard.tap(Key.right)
        elif data == 'F1': self.keyboard.tap(Key.f1)
        elif data == 'F2': self.keyboard.tap(Key.f2)
        elif data == 'F3': self.keyboard.tap(Key.f3)
        elif data == 'F4': self.keyboard.tap(Key.f4)
        elif data == 'F5': self.keyboard.tap(Key.f5)
        elif data == 'F6': self.keyboard.tap(Key.f6)
        elif data == 'F7': self.keyboard.tap(Key.f7)
        elif data == 'F8': self.keyboard.tap(Key.f8)
        elif data == 'F9': self.keyboard.tap(Key.f9)
        elif data == 'F10': self.keyboard.tap(Key.f10)
        elif data == 'F11': self.keyboard.tap(Key.f11)
        elif data == 'F12': self.keyboard.tap(Key.f12)

        elif data == 'VOLUMEUP':
            self.keyboard.tap(Key.media_volume_up)
        elif data == 'VOLUMEDOWN':
            self.keyboard.tap(Key.media_volume_down)
        elif data == 'MUTE':
            self.keyboard.tap(Key.media_volume_mute)

        elif data.startswith('CLIPBOARD|'):
            text = data[10:]
            self.recentClipboard = text
            pyperclip.copy(text)

        elif data.startswith('TEXT|'):
            text = data[5:]
            self.keyboard.type(text)

        else:
            splitter = data.split('|')
            if(len(splitter) == 3):
                if(splitter[0] == 'S' and self.updatePointer):
                    self.evtPointerMove.emit(float(splitter[1]), float(splitter[2]))
                elif(splitter[0] == 'M'):
                    dx = float(splitter[1]) * self.mouseSpeed
                    dy = float(splitter[2]) * self.mouseSpeed
                    self.mouse.move(dx, dy)

class AboutWindow(QtWidgets.QDialog):
    def __init__(self, *args, **kwargs):
        super(AboutWindow, self).__init__(*args, **kwargs)

        self.buttonBox = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.StandardButton.Ok)
        self.buttonBox.accepted.connect(self.accept)

        self.layout = QtWidgets.QVBoxLayout(self)

        labelAppName = QtWidgets.QLabel(self)
        labelAppName.setText(APP_NAME + ' v' + APP_VERSION)
        labelAppName.setStyleSheet('font-weight:bold')
        labelAppName.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(labelAppName)

        labelCopyright = QtWidgets.QLabel(self)
        labelCopyright.setText(
            '<br>'
            '© 2018-2026 <a href="https://georg-sieber.de">Georg Sieber</a>'
            '<br>'
            '<br>'
            'GNU General Public License v3.0'
            '<br>'
            '<br>'
            +QtWidgets.QApplication.translate(APP_NAME, 'Use the RemotePointer Android app to control you computer with this software.')
            +'<br>'
            '<br>'
            +QtWidgets.QApplication.translate(APP_NAME, 'If you like RemotePointer, please consider making a donation to support further development.')
            +'<br>'
        )
        labelCopyright.setOpenExternalLinks(True)
        labelCopyright.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(labelCopyright)

        labelDescription = QtWidgets.QLabel(self)
        labelDescription.setText('')
        labelDescription.setFixedWidth(450)
        labelDescription.setWordWrap(True)
        labelDescription.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(labelDescription)

        self.layout.addWidget(self.buttonBox)

        self.setLayout(self.layout)
        self.setWindowTitle(APP_NAME)

class SystemTrayIcon(QtWidgets.QSystemTrayIcon):
    def __init__(self, icon, parent):
        QtWidgets.QSystemTrayIcon.__init__(self, icon, parent)
        self.parentWidget = parent
        menu = QtWidgets.QMenu(parent)
        openAction = menu.addAction(QtWidgets.QApplication.translate(APP_NAME, 'Show Main Window'))
        openAction.triggered.connect(self.open)
        exitAction = menu.addAction(QtWidgets.QApplication.translate(APP_NAME, '&Quit'))
        exitAction.triggered.connect(self.exit)
        self.setContextMenu(menu)
        self.activated.connect(self.showMenuOnTrigger)
        self.setToolTip(APP_NAME)

    def showMenuOnTrigger(self, reason):
        if(reason == QtWidgets.QSystemTrayIcon.ActivationReason.Trigger):
            self.open()
        elif(reason == QtWidgets.QSystemTrayIcon.ActivationReason.Context):
            self.contextMenu().popup(QtGui.QCursor.pos())

    def open(self):
        self.parentWidget.show()

    def exit(self):
        self.parentWidget.close()
        QtCore.QCoreApplication.exit()

class MainWindow(QtWidgets.QMainWindow):
    evtPointerShowHide = QtCore.pyqtSignal(bool)
    evtPointerMove = QtCore.pyqtSignal(float, float)

    def __init__(self, config, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.config = config

        # register event handler
        self.pointerWindow = PointerWindow()
        self.evtPointerShowHide.connect(self.evtPointerShowHideHandler)
        self.evtPointerMove.connect(self.evtPointerMoveHandler)

        # icons
        self.pathImgLogo = os.path.dirname(os.path.realpath(__file__))+'/res/icon.png'
        if(self.isDarkMode(self.palette())):
            self.pathImgHost = os.path.dirname(os.path.realpath(__file__))+'/res/computer.light.png'
            self.pathImgKey = os.path.dirname(os.path.realpath(__file__))+'/res/key.light.png'
        else:
            self.pathImgHost = os.path.dirname(os.path.realpath(__file__))+'/res/computer.png'
            self.pathImgKey = os.path.dirname(os.path.realpath(__file__))+'/res/key.png'

        # window layout
        grid = QtWidgets.QGridLayout()
        grid.setColumnStretch(0, 1)
        grid.setColumnStretch(1, 3)

        bigFont = QtGui.QFont()
        bigFont.setPointSize(16)
        bigFont.setWeight(QtGui.QFont.Weight.Bold);

        self.imgHost = QtWidgets.QLabel()
        self.imgHost.setPixmap(QtGui.QPixmap(self.pathImgHost))
        grid.addWidget(self.imgHost, 0, 0)
        self.lblHost = QtWidgets.QLabel(self.getHostname())
        self.lblHost.setFont(bigFont)
        grid.addWidget(self.lblHost, 0, 1)

        self.imgHost = QtWidgets.QLabel()
        self.imgHost.setPixmap(QtGui.QPixmap(self.pathImgKey))
        grid.addWidget(self.imgHost, 1, 0)
        self.lblKey = QtWidgets.QLabel(self.config['server']['code'])
        self.lblKey.setFont(bigFont)
        grid.addWidget(self.lblKey, 1, 1)

        widget = QtWidgets.QWidget(self)
        widget.setLayout(grid)
        self.setCentralWidget(widget)

        # Menubar
        mainMenu = self.menuBar()

        # Options Menu
        optionsMenu = mainMenu.addMenu(QtWidgets.QApplication.translate(APP_NAME, '&Options'))

        laserpointerAction = optionsMenu.addMenu(QtWidgets.QApplication.translate(APP_NAME, '&Laserpointer Style'))
        laserpointerStyleActionGroup = QtGui.QActionGroup(self)
        laserpointerStyleActionGroup.setExclusive(True)
        for file in glob.glob(POINTERS_DIR+'/*'):
            actionButton = laserpointerStyleActionGroup.addAction(QtGui.QAction(Path(file).stem, self, checkable=True))
            actionButton.triggered.connect(partial(self.clickLaserpointerStyle, file, actionButton))
            actionButton.style = file
            if(file == config['pointer'].get('style')):
                actionButton.setChecked(True)
            laserpointerAction.addAction(actionButton)

        laserpointerSpeedAction = optionsMenu.addMenu(QtWidgets.QApplication.translate(APP_NAME, 'Laserpointer &Speed'))
        laserpointerSpeedActionGroup = QtGui.QActionGroup(self)
        laserpointerSpeedActionGroup.setExclusive(True)
        for i in range(1, 80, 4):
            actionButton = laserpointerSpeedActionGroup.addAction(QtGui.QAction(str(i), self, checkable=True))
            actionButton.triggered.connect(partial(self.clickLaserpointerSpeed, i, actionButton))
            actionButton.speed = i
            if(str(i) == config['pointer'].get('speed')):
                actionButton.setChecked(True)
            laserpointerSpeedAction.addAction(actionButton)

        mouseSpeedAction = optionsMenu.addMenu(QtWidgets.QApplication.translate(APP_NAME, '&Mouse Speed'))
        mouseSpeedActionGroup = QtGui.QActionGroup(self)
        mouseSpeedActionGroup.setExclusive(True)
        for i in range(1, 10):
            actionButton = mouseSpeedActionGroup.addAction(QtGui.QAction(str(i), self, checkable=True))
            actionButton.triggered.connect(partial(self.clickMouseSpeed, i, actionButton))
            actionButton.speed = i
            if(str(i) == config['mouse'].get('speed')):
                actionButton.setChecked(True)
            mouseSpeedAction.addAction(actionButton)

        optionsMenu.addSeparator()
        newCodeAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, '&Generate New Code'), self)
        newCodeAction.triggered.connect(self.clickNewCode)
        optionsMenu.addAction(newCodeAction)
        optionsMenu.addSeparator()
        quitAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, '&Quit'), self)
        quitAction.setShortcut('Ctrl+Q')
        quitAction.triggered.connect(self.clickQuit)
        optionsMenu.addAction(quitAction)

        # Help Menu
        helpMenu = mainMenu.addMenu(QtWidgets.QApplication.translate(APP_NAME, '&Help'))
        playStoreAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, 'Android app in Play Store'), self)
        playStoreAction.triggered.connect(self.clickPlayStore)
        helpMenu.addAction(playStoreAction)
        githubAndroidAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, 'Android app on Github'), self)
        githubAndroidAction.triggered.connect(self.clickGithubAndroid)
        helpMenu.addAction(githubAndroidAction)
        githubServerAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, 'Server app on Github'), self)
        githubServerAction.triggered.connect(self.clickGithubServer)
        helpMenu.addAction(githubServerAction)
        helpMenu.addSeparator()
        aboutAction = QtGui.QAction(QtWidgets.QApplication.translate(APP_NAME, '&About'), self)
        aboutAction.triggered.connect(self.clickAbout)
        helpMenu.addAction(aboutAction)

        # window properties
        self.setWindowTitle(APP_NAME)
        self.resize(350, 150)
        self.setWindowIcon(QtGui.QIcon(self.pathImgLogo))
        # this flag combination leads to only a MinimizeButton in title bar
        self.setWindowFlag(QtCore.Qt.WindowType.WindowMinimizeButtonHint, True)
        self.setWindowFlag(QtCore.Qt.WindowType.WindowMaximizeButtonHint, False)
        self.setWindowFlag(QtCore.Qt.WindowType.Dialog, False)

        # tray icon
        self.trayIcon = SystemTrayIcon(QtGui.QIcon(), self)
        self.trayIcon.setIcon(QtGui.QIcon(self.pathImgLogo))
        self.trayIcon.show()

        # start broadcasting
        self.broadcaster = AdvertiseBroadcast(
            int(self.config['server']['broadcastPort']),
            int(self.config['server']['listenPort']),
            self.getHostname()
        )
        self.broadcaster.start()

        # start listening for incoming connections
        self.listener = ControlListener(
            int(self.config['server']['listenPort']),
            self.config['server']['code'],
            self.evtPointerShowHide,
            self.evtPointerMove,
            int(self.config['mouse']['speed']),
        )
        self.listener.start()

        # center screen
        qr = self.frameGeometry()
        cp = self.screen().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def isDarkMode(self, palette):
        return (palette.color(QtGui.QPalette.ColorRole.Window).red() < 100
            and palette.color(QtGui.QPalette.ColorRole.Window).green() < 100
            and palette.color(QtGui.QPalette.ColorRole.Window).blue() < 100)

    def evtPointerShowHideHandler(self, visible):
        if(visible == True):
            self.pointerWindow.show()
            self.pointerWindow.img.setPixmap(QtGui.QPixmap(self.config['pointer']['style']))
        elif(visible == False):
            self.pointerWindow.hide()
    def evtPointerMoveHandler(self, dx, dy):
        if(dx and dy):
            dx = dx * int(self.config['pointer']['speed'])
            dy = dy * int(self.config['pointer']['speed'])
            currentPos = self.pointerWindow.pos()
            self.pointerWindow.move(int(currentPos.x() + dx), int(currentPos.y() + dy))

    def getHostname(self):
        if(self.config['server'].get('hostname')):
            return self.config['server'].get('hostname')
        else:
            return socket.gethostname()

    def clickNewCode(self):
        newCode = generateCode()
        self.config.set('server', 'code', newCode)
        saveSettings(self.config)
        self.listener.code = newCode
        self.lblKey.setText(newCode)

    def clickLaserpointerStyle(self, key, button):
        self.config.set('pointer', 'style', str(key))
        saveSettings(self.config)
        self.listener.pointerStyle = key
    def clickLaserpointerSpeed(self, key, button):
        self.config.set('pointer', 'speed', str(key))
        saveSettings(self.config)
        self.listener.pointerSpeed = key
    def clickMouseSpeed(self, key, button):
        self.config.set('mouse', 'speed', str(key))
        saveSettings(self.config)
        self.listener.mouseSpeed = key

    def clickAbout(self, e):
        dlg = AboutWindow(self)
        dlg.exec()
    def clickGithubServer(self, e):
        webbrowser.open('https://github.com/schorschii/RemotePointer-Server')
    def clickGithubAndroid(self, e):
        webbrowser.open('https://github.com/schorschii/RemotePointer-Android')
    def clickPlayStore(self, e):
        webbrowser.open('https://play.google.com/store/apps/details?id=systems.sieber.remotespotlight')

    def clickQuit(self, e):
        self.close()
        QtCore.QCoreApplication.exit()

def generateCode():
    return ''.join(secrets.choice('0123456789') for i in range(4))

def saveSettings(config):
    with open(CONFIG_FILE, 'w') as f:
        config.write(f)

if __name__ == '__main__':
    # get config
    config = configparser.ConfigParser()
    config.read(CONFIG_FILE)
    # apply defaults
    for sect, values in DEFAULT_CONFIG.items():
        if(not config.has_section(sect)):
            config.add_section(sect)
        for key, value in values.items():
            if(not key in config[sect]):
                config.set(sect, key, str(value))
    # generate code if empty (first run)
    if(not config['server'].get('code')):
        config.set('server', 'code', generateCode())
    # set red pointer as default if empty (first run)
    # path separators vary if on win32, that's why not working in DEFAULT_CONFIG
    if(not os.path.exists(config['pointer'].get('style'))):
        allPointers = glob.glob(POINTERS_DIR+'/*')
        if(allPointers):
            for pointer in allPointers:
                if('red' in pointer.lower()):
                    config.set('pointer', 'style', pointer)
            if(not config['pointer'].get('style')):
                config.set('pointer', 'style', allPointers[0])
    saveSettings(config)

    # parse cmd args
    parser = argparse.ArgumentParser()
    parser.add_argument('--hidden', '-q', action='store_true', help='Start without opening the main window (for autostart)')
    args = parser.parse_args()

    # init QT app
    app = QtWidgets.QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(False)

    # load QT translations
    translator = QtCore.QTranslator(app)
    if getattr(sys, 'frozen', False):
        translator.load(os.path.join(sys._MEIPASS, 'lang/%s.qm' % getlocale()[0]))
    elif os.path.isdir('lang'):
        translator.load('lang/%s.qm' % getlocale()[0])
    else:
        translator.load('/usr/share/remotepointer/lang/%s.qm' % getlocale()[0])
    app.installTranslator(translator)

    # start main window
    window = MainWindow(config)
    if not args.hidden: window.show()
    exitCode = app.exec()
    sys.exit(exitCode)
