/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package RemotePointerServer;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.prefs.Preferences;

public class AboutWindow extends javax.swing.JFrame {

    private RuntimeSettings rs;

    public AboutWindow() {
        initComponents();
    }
    public AboutWindow(RuntimeSettings rs) {
        this.rs = rs;
        initComponents();
        initSliders();
    }
    private void initSliders() {
        jTextAreaAbout.setCaretPosition(0);
        if(rs != null) {
            jComboBoxStyle.setSelectedIndex(rs.pointerStyle);
            float ms = rs.mouseSpeed*10;
            jSliderPointerSpeed.setValue((int)rs.pointerSpeed);
            jSliderMouseSpeed.setValue((int)ms);
        }
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jScrollPane1 = new javax.swing.JScrollPane();
        jTextAreaAbout = new javax.swing.JTextArea();
        jButtonClose = new javax.swing.JButton();
        jButtonWebsite = new javax.swing.JButton();
        jButtonPlayStore = new javax.swing.JButton();
        jLabelMouseSpeed = new javax.swing.JLabel();
        jSliderPointerSpeed = new javax.swing.JSlider();
        jSliderMouseSpeed = new javax.swing.JSlider();
        jLabelVersion = new javax.swing.JLabel();
        jLabelPointerSpeed = new javax.swing.JLabel();
        jLabelPointerSpeed1 = new javax.swing.JLabel();
        jComboBoxStyle = new javax.swing.JComboBox<>();
        jButtonClose1 = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        java.util.ResourceBundle bundle = java.util.ResourceBundle.getBundle("RemotePointerServer/Bundle"); // NOI18N
        setTitle(bundle.getString("AboutWindow.title")); // NOI18N
        setMinimumSize(new java.awt.Dimension(550, 445));

        jTextAreaAbout.setEditable(false);
        jTextAreaAbout.setColumns(20);
        jTextAreaAbout.setLineWrap(true);
        jTextAreaAbout.setRows(5);
        jTextAreaAbout.setText(bundle.getString("AboutWindow.jTextAreaAbout.text")); // NOI18N
        jTextAreaAbout.setWrapStyleWord(true);
        jScrollPane1.setViewportView(jTextAreaAbout);

        jButtonClose.setText(bundle.getString("AboutWindow.jButtonClose.text")); // NOI18N
        jButtonClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonCloseActionPerformed(evt);
            }
        });

        jButtonWebsite.setText(bundle.getString("AboutWindow.jButtonWebsite.text")); // NOI18N
        jButtonWebsite.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonWebsiteActionPerformed(evt);
            }
        });

        jButtonPlayStore.setText(bundle.getString("AboutWindow.jButtonPlayStore.text")); // NOI18N
        jButtonPlayStore.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonPlayStoreActionPerformed(evt);
            }
        });

        jLabelMouseSpeed.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jLabelMouseSpeed.setText(bundle.getString("AboutWindow.jLabelMouseSpeed.text")); // NOI18N
        jLabelMouseSpeed.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);

        jSliderPointerSpeed.setMaximum(80);
        jSliderPointerSpeed.setMinimum(10);
        jSliderPointerSpeed.setValue(25);
        jSliderPointerSpeed.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                onPointerSpeedChanged(evt);
            }
        });

        jSliderMouseSpeed.setMaximum(30);
        jSliderMouseSpeed.setMinimum(5);
        jSliderMouseSpeed.setValue(10);
        jSliderMouseSpeed.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                onMouseSpeedChanged(evt);
            }
        });

        jLabelVersion.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jLabelVersion.setText(bundle.getString("AboutWindow.jLabelVersion.text")); // NOI18N
        jLabelVersion.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);

        jLabelPointerSpeed.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jLabelPointerSpeed.setText(bundle.getString("AboutWindow.jLabelPointerSpeed.text")); // NOI18N
        jLabelPointerSpeed.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);

        jLabelPointerSpeed1.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        jLabelPointerSpeed1.setText(bundle.getString("AboutWindow.jLabelPointerSpeed1.text")); // NOI18N
        jLabelPointerSpeed1.setHorizontalTextPosition(javax.swing.SwingConstants.RIGHT);

        jComboBoxStyle.setModel(new javax.swing.DefaultComboBoxModel<>(new String[] { "Red", "Green", "Blue", "Spotlight", "Arrow" }));
        jComboBoxStyle.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jComboBoxStyleActionPerformed(evt);
            }
        });

        jButtonClose1.setText(bundle.getString("AboutWindow.jButtonClose1.text")); // NOI18N
        jButtonClose1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButtonClose1ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane1)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(jButtonWebsite)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jLabelVersion)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButtonClose))
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jButtonPlayStore)
                            .addComponent(jLabelMouseSpeed)
                            .addComponent(jLabelPointerSpeed)
                            .addComponent(jLabelPointerSpeed1))
                        .addGap(18, 18, 18)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jComboBoxStyle, javax.swing.GroupLayout.Alignment.TRAILING, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jSliderPointerSpeed, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jSliderMouseSpeed, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                                .addGap(0, 0, Short.MAX_VALUE)
                                .addComponent(jButtonClose1)))))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 165, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jComboBoxStyle, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabelPointerSpeed1, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jSliderPointerSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabelPointerSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jSliderMouseSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabelMouseSpeed, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButtonPlayStore)
                    .addComponent(jButtonClose1))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButtonClose)
                    .addComponent(jButtonWebsite)
                    .addComponent(jLabelVersion, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButtonCloseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonCloseActionPerformed
        Preferences prefs = Preferences.userNodeForPackage(RemotePointerServer.RemoteSpotlight.class);
        prefs.put("pointer_style", Integer.toString(jComboBoxStyle.getSelectedIndex()));
        prefs.put("pointer_speed", Float.toString(jSliderPointerSpeed.getValue()));
        prefs.put("mouse_speed", Float.toString((float)jSliderMouseSpeed.getValue()/10));

        this.setVisible(false);
        this.dispose();
    }//GEN-LAST:event_jButtonCloseActionPerformed

    private void jButtonPlayStoreActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonPlayStoreActionPerformed
        try {
            java.awt.Desktop.getDesktop().browse(new URI("https://play.google.com/store/apps/details?id=systems.sieber.remotespotlight"));
        } catch (IOException ex) {
            Logger.getLogger(AboutWindow.class.getName()).log(Level.SEVERE, null, ex);
        } catch (URISyntaxException ex) {
            Logger.getLogger(AboutWindow.class.getName()).log(Level.SEVERE, null, ex);
        }
    }//GEN-LAST:event_jButtonPlayStoreActionPerformed

    private void jButtonWebsiteActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonWebsiteActionPerformed
        try {
            java.awt.Desktop.getDesktop().browse(new URI("https://georg-sieber.de"));
        } catch (IOException ex) {
            Logger.getLogger(AboutWindow.class.getName()).log(Level.SEVERE, null, ex);
        } catch (URISyntaxException ex) {
            Logger.getLogger(AboutWindow.class.getName()).log(Level.SEVERE, null, ex);
        }
    }//GEN-LAST:event_jButtonWebsiteActionPerformed

    private void onPointerSpeedChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_onPointerSpeedChanged
        this.rs.pointerSpeed = jSliderPointerSpeed.getValue();
    }//GEN-LAST:event_onPointerSpeedChanged

    private void onMouseSpeedChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_onMouseSpeedChanged
        this.rs.mouseSpeed = (float)jSliderMouseSpeed.getValue()/10;
    }//GEN-LAST:event_onMouseSpeedChanged

    private void jComboBoxStyleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jComboBoxStyleActionPerformed
        this.rs.pointerStyle = jComboBoxStyle.getSelectedIndex();
    }//GEN-LAST:event_jComboBoxStyleActionPerformed

    private void jButtonClose1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButtonClose1ActionPerformed
        Preferences prefs = Preferences.userNodeForPackage(RemotePointerServer.RemoteSpotlight.class);
        prefs.remove("connection_code");
        System.exit(0);
    }//GEN-LAST:event_jButtonClose1ActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(AboutWindow.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(AboutWindow.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(AboutWindow.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(AboutWindow.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new AboutWindow().setVisible(true);
            }
        });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton jButtonClose;
    private javax.swing.JButton jButtonClose1;
    private javax.swing.JButton jButtonPlayStore;
    private javax.swing.JButton jButtonWebsite;
    private javax.swing.JComboBox<String> jComboBoxStyle;
    private javax.swing.JLabel jLabelMouseSpeed;
    private javax.swing.JLabel jLabelPointerSpeed;
    private javax.swing.JLabel jLabelPointerSpeed1;
    private javax.swing.JLabel jLabelVersion;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JSlider jSliderMouseSpeed;
    private javax.swing.JSlider jSliderPointerSpeed;
    private javax.swing.JTextArea jTextAreaAbout;
    // End of variables declaration//GEN-END:variables
}
