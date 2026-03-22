/*
CORE RULE:
Every piece of logic has one clear owner. No duplication.

LAYERS:
- UI: Presentation only
- App: Orchestration / Coordination
- Logic: Business rules / Calculations
- Integration: Device / SFOS APIs (Sailjail compliant)
- Persistence: Storage / Serialization

RULES:
- Simplicity: Prefer explicit, readable code over abstractions
- Stability: Preserve behavior; modify over rewrite; no silent failures
- Platform: Design for failure/limits; background tasks are unreliable
- IDE: New files must be added in a project-aware, build-aware way (Qt Creator)

MANDATORY DRY-RUN (Pre-Code):
- Visibility: Seen (pasted) vs Inferred (not seen)
- Size: Micro / Scoped / Structural
- Placement: File path + exact anchor point
- Layers: Affected layer(s)

FILE & CONTEXT RULE:
- Always name the file when referring to code
- No floating code - all code must have explicit placement
- Provide enough surrounding context to make changes unambiguous
- If code is not pasted, it is not known - do not assume or invent structure
- If unsure, ask instead of guessing
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0

Page {
    id: page

    property string lastNotifyEvent: "none"

    allowedOrientations: Orientation.All

    Notification {
        id: runtimeNotification
        appName: "Pling"
        summary: "Pling"
        body: ""
        category: "x-nemo.messaging.im"
        soundName: "message-new-instant"
    }

    Connections {
        target: chatChecker

        onNotifyRequested: {
            page.lastNotifyEvent = summaryText + " | " + bodyText
            runtimeNotification.summary = summaryText
            runtimeNotification.body = bodyText
            runtimeNotification.publish()
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Login and check now")
                onClicked: chatChecker.loginAndCheckNow()
            }

            MenuItem {
                text: qsTr("Check now")
                onClicked: chatChecker.checkNow()
            }

            MenuItem {
                text: qsTr("Login")
                onClicked: chatChecker.login()
            }

            MenuItem {
                text: qsTr("Send test notification")
                onClicked: {
                    runtimeNotification.summary = qsTr("Pling")
                    runtimeNotification.body = qsTr("Manual test notification")
                    runtimeNotification.publish()
                }
            }

            MenuItem {
                text: qsTr("Reset session")
                onClicked: chatChecker.resetSession()
            }
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Pling")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Log in, then check one chat manually.")
                color: Theme.secondaryHighlightColor
                wrapMode: Text.Wrap
            }

            TextField {
                id: baseUrlField
                width: parent.width
                label: qsTr("Base URL")
                text: chatChecker.baseUrl
                onTextChanged: chatChecker.baseUrl = text
            }

            TextField {
                id: userField
                width: parent.width
                label: qsTr("User")
                text: chatChecker.user
                onTextChanged: chatChecker.user = text
            }

            TextField {
                id: passwordField
                width: parent.width
                label: qsTr("Password")
                echoMode: TextInput.Password
                text: chatChecker.password
                onTextChanged: chatChecker.password = text
            }

            TextField {
                id: chatField
                width: parent.width
                label: qsTr("Chat")
                text: chatChecker.chat
                onTextChanged: chatChecker.chat = text
            }

            SectionHeader {
                text: qsTr("State")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Logged in: %1").arg(chatChecker.loggedIn ? qsTr("yes") : qsTr("no"))
                wrapMode: Text.Wrap
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Busy: %1").arg(chatChecker.busy ? qsTr("yes") : qsTr("no"))
                wrapMode: Text.Wrap
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Last seen id: %1").arg(chatChecker.lastSeenId)
                wrapMode: Text.Wrap
            }

            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Status: %1").arg(chatChecker.status)
                wrapMode: Text.Wrap
            }
            Label {
                x: Theme.horizontalPageMargin
                width: page.width - 2 * Theme.horizontalPageMargin
                text: qsTr("Last notify event: %1").arg(page.lastNotifyEvent)
                wrapMode: Text.Wrap
            }
        }
    }
}
