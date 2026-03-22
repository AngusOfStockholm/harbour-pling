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

    allowedOrientations: Orientation.All

    Notification {
        id: testNotification
        appName: "Pling"
        summary: qsTr("Pling")
        body: qsTr("Test notification from harbour-pling")
        category: "x-nemo.messaging.im"
        soundName: "message-new-instant"
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Send test notification")
                onClicked: testNotification.publish()
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
                text: qsTr("Pull down to send a test notification.")
                color: Theme.secondaryHighlightColor
                wrapMode: Text.Wrap
            }
        }
    }
}
