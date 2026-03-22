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
import "pages"

ApplicationWindow {
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
