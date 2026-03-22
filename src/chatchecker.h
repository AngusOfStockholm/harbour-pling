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

#ifndef CHATCHECKER_H
#define CHATCHECKER_H

#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;
class QNetworkCookieJar;
class QTimer;

class ChatChecker : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString chat READ chat WRITE setChat NOTIFY chatChanged)
    Q_PROPERTY(int lastSeenId READ lastSeenId NOTIFY lastSeenIdChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit ChatChecker(QObject *parent = nullptr);

    QString baseUrl() const;
    void setBaseUrl(const QString &value);

    QString user() const;
    void setUser(const QString &value);

    QString password() const;
    void setPassword(const QString &value);

    QString chat() const;
    void setChat(const QString &value);

    int lastSeenId() const;
    QString status() const;
    bool loggedIn() const;
    bool busy() const;

    Q_INVOKABLE void login();
    Q_INVOKABLE void checkNow();
    Q_INVOKABLE void loginAndCheckNow();
    Q_INVOKABLE void resetSession();

signals:
    void baseUrlChanged();
    void userChanged();
    void passwordChanged();
    void chatChanged();
    void lastSeenIdChanged();
    void statusChanged();
    void loggedInChanged();
    void busyChanged();

    void notifyRequested(const QString &summaryText, const QString &bodyText);

private:
    void setStatus(const QString &value);
    void setLoggedIn(bool value);
    void setBusy(bool value);
    void setLastSeenId(int value);

    void handleLoginReply(QNetworkReply *reply, bool thenCheck);
    void handlePollReply(QNetworkReply *reply);

private:
    QNetworkAccessManager m_network;
    QNetworkCookieJar *m_cookieJar;
    QTimer *m_pollTimer;

    QString m_baseUrl;
    QString m_user;
    QString m_password;
    QString m_chat;
    int m_lastSeenId;
    QString m_status;
    bool m_loggedIn;
    bool m_busy;
};

#endif // CHATCHECKER_H
