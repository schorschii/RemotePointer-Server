#!/bin/bash
set -e

# parameters
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
TARGET_DIRECTORY="$SCRIPTPATH/target"
PRODUCT="RemotePointer"
VERSION="3.2"
TARGET_FILENAME="RemotePointer.pkg"
#DEVELOPER_ID_INSTALLER_CERT_NAME=""
#DEVELOPER_ACCOUNT_USERNAME=""
#DEVELOPER_ACCOUNT_PASSWORD=""
#DEVELOPER_ACCOUNT_TEAM=""


# create/clean target build directory
if [ -d "${TARGET_DIRECTORY}" ]; then
	sudo rm -rf "$TARGET_DIRECTORY"
fi
mkdir -p "$TARGET_DIRECTORY"

cp -r "$SCRIPTPATH/darwin" "${TARGET_DIRECTORY}/"
#chmod -R 755 "${TARGET_DIRECTORY}/darwin/scripts"
chmod -R 755 "${TARGET_DIRECTORY}/darwin/Resources"
chmod 644 "${TARGET_DIRECTORY}/darwin/Distribution"


# prepare installer files
sed -i '' -e "s/__VERSION__/$VERSION/g" "${TARGET_DIRECTORY}/darwin/Distribution"
sed -i '' -e "s/__PRODUCT__/$PRODUCT/g" "${TARGET_DIRECTORY}/darwin/Distribution"
sed -i '' -e "s/__FILENAME__/$TARGET_FILENAME/g" "${TARGET_DIRECTORY}/darwin/Distribution"

#sed -i '' -e "s/__VERSION__/$VERSION/g" "${TARGET_DIRECTORY}"/darwin/Resources/*.html
#sed -i '' -e "s/__PRODUCT__/$PRODUCT/g" "${TARGET_DIRECTORY}"/darwin/Resources/*.html


# copy files in place
mkdir -p "${TARGET_DIRECTORY}/darwinpkg"
mkdir -p "${TARGET_DIRECTORY}/darwinpkg/Applications"
mkdir -p "${TARGET_DIRECTORY}/darwinpkg/Library/LaunchAgents"
cp -R "../../dist/RemotePointer.app" "${TARGET_DIRECTORY}/darwinpkg/Applications"
cp "systems.sieber.remotespotlight.plist" "${TARGET_DIRECTORY}/darwinpkg/Library/LaunchAgents/"
sudo chown -R root:wheel "${TARGET_DIRECTORY}/darwinpkg"

rm -rf "${TARGET_DIRECTORY}/package"
mkdir -p "${TARGET_DIRECTORY}/package"
chmod 755 "${TARGET_DIRECTORY}/package"

rm -rf "${TARGET_DIRECTORY}/pkg"
mkdir -p "${TARGET_DIRECTORY}/pkg"
chmod 755 "${TARGET_DIRECTORY}/pkg"

find "${TARGET_DIRECTORY}" -name ".DS_Store" -delete


# sign executable (done by PyInstaller)
#if [ "$DEVELOPER_ID_INSTALLER_CERT_NAME" != "" ]; then
#	echo "Sign executable ..."
#	codesign --deep --force --options=runtime --entitlements ./entitlements.plist --sign "$DEVELOPER_ID_INSTALLER_CERT_NAME" --timestamp ./target/darwinpkg/opt/oco-agent/oco-agent
#fi


# build packages
echo "Build application installer package ..."
pkgbuild --identifier "org.${PRODUCT}.${VERSION}" \
	--version "${VERSION}" \
	--scripts "${TARGET_DIRECTORY}/darwin/scripts" \
	--root "${TARGET_DIRECTORY}/darwinpkg" \
	${DEVELOPER_ID_INSTALLER_CERT_NAME:+"--sign" "$DEVELOPER_ID_INSTALLER_CERT_NAME"} \
	"${TARGET_DIRECTORY}/package/$TARGET_FILENAME"


# build application installer
echo "Build application installer product ..."
productbuild --distribution "${TARGET_DIRECTORY}/darwin/Distribution" \
	--resources "${TARGET_DIRECTORY}/darwin/Resources" \
	--package-path "${TARGET_DIRECTORY}/package" \
	"${TARGET_DIRECTORY}/pkg/$TARGET_FILENAME"


# sign outer package
if [ "$DEVELOPER_ID_INSTALLER_CERT_NAME" != "" ]; then
	echo "Sign application installer ..."
	mkdir -pv "${TARGET_DIRECTORY}/pkg-signed"
	chmod 755 "${TARGET_DIRECTORY}/pkg-signed"

	productsign --sign "${DEVELOPER_ID_INSTALLER_CERT_NAME}" \
		"${TARGET_DIRECTORY}/pkg/$TARGET_FILENAME" \
		"${TARGET_DIRECTORY}/pkg-signed/$TARGET_FILENAME"

	pkgutil --check-signature "${TARGET_DIRECTORY}/pkg-signed/$TARGET_FILENAME"
fi


# cleanup
rm -rf "${TARGET_DIRECTORY}/package"

# notarize (only possible with valid signature)
if [ "$DEVELOPER_ID_INSTALLER_CERT_NAME" != "" ]; then
	if [ "$DEVELOPER_ACCOUNT_USERNAME" != "" ] && [ "$DEVELOPER_ACCOUNT_PASSWORD" != "" ] && [ "$DEVELOPER_ACCOUNT_TEAM" != "" ]; then
		echo "Store credentials for notarization ..."
		xcrun notarytool store-credentials "notarytool-password" --apple-id "$DEVELOPER_ACCOUNT_USERNAME" --password "$DEVELOPER_ACCOUNT_PASSWORD" --team-id "$DEVELOPER_ACCOUNT_TEAM"
	fi

	echo "Notarize application installer ..."
	xcrun notarytool submit "${TARGET_DIRECTORY}/pkg-signed/oco-agent.pkg" --wait --keychain-profile "notarytool-password"

	# get logfile with additional information:
	# xcrun notarytool log --keychain-profile "notarytool-password" xxx-xxx-xxx-xxx developer_log.json
fi


echo "Build finished"
