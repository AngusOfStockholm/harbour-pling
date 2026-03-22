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

#include "chatchecker.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>

namespace {

static const char *SETTINGS_GROUP = "ChatChecker";
static const char *KEY_BASE_URL = "baseUrl";
static const char *KEY_USER = "user";
static const char *KEY_CHAT = "chat";
static const char *KEY_LAST_SEEN_ID = "lastSeenId";



void saveBaseUrlSetting(const QString &value)
{
    QSettings settings("harbour-pling", "harbour-pling");
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(KEY_BASE_URL, value);
    settings.endGroup();
}

void saveUserSetting(const QString &value)
{
    QSettings settings("harbour-pling", "harbour-pling");
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(KEY_USER, value);
    settings.endGroup();
}

void saveChatSetting(const QString &value)
{
    QSettings settings("harbour-pling", "harbour-pling");
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(KEY_CHAT, value);
    settings.endGroup();
}

void saveLastSeenIdSetting(int value)
{
    QSettings settings("harbour-pling", "harbour-pling");
    settings.beginGroup(SETTINGS_GROUP);
    settings.setValue(KEY_LAST_SEEN_ID, value);
    settings.endGroup();
}

}

ChatChecker::ChatChecker(QObject *parent)
    : QObject(parent)
    , m_cookieJar(new QNetworkCookieJar(this))
    , m_baseUrl("https://chat.dotdot.se")
    , m_lastSeenId(0)
    , m_loggedIn(false)
    , m_busy(false)
{
    m_network.setCookieJar(m_cookieJar);
    m_chat = "family";
    m_status = "Ready";

    QSettings settings("harbour-pling", "harbour-pling");
    settings.beginGroup(SETTINGS_GROUP);

    m_baseUrl = settings.value(KEY_BASE_URL, m_baseUrl).toString();
    m_user = settings.value(KEY_USER, m_user).toString();
    m_chat = settings.value(KEY_CHAT, m_chat).toString();
    m_lastSeenId = settings.value(KEY_LAST_SEEN_ID, m_lastSeenId).toInt();

    settings.endGroup();

    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(60000); // 60 seconds

    connect(m_pollTimer, &QTimer::timeout, this, [this]() {
        if (m_busy)
            return;

        if (!m_loggedIn)
            return;

        if (m_chat.isEmpty())
            return;

        checkNow();
    });
}

QString ChatChecker::baseUrl() const
{
    return m_baseUrl;
}

void ChatChecker::setBaseUrl(const QString &value)
{
    if (m_baseUrl == value)
        return;

    m_baseUrl = value;
    saveBaseUrlSetting(m_baseUrl);
    emit baseUrlChanged();
}

QString ChatChecker::user() const
{
    return m_user;
}

void ChatChecker::setUser(const QString &value)
{
    if (m_user == value)
        return;

    m_user = value;
    saveUserSetting(m_user);
    emit userChanged();
}

QString ChatChecker::password() const
{
    return m_password;
}

void ChatChecker::setPassword(const QString &value)
{
    if (m_password == value)
        return;

    m_password = value;
    emit passwordChanged();
}

QString ChatChecker::chat() const
{
    return m_chat;
}

void ChatChecker::setChat(const QString &value)
{
    if (m_chat == value)
        return;

    m_chat = value;
    saveChatSetting(m_chat);
    emit chatChanged();
}

int ChatChecker::lastSeenId() const
{
    return m_lastSeenId;
}

QString ChatChecker::status() const
{
    return m_status;
}

bool ChatChecker::loggedIn() const
{
    return m_loggedIn;
}

bool ChatChecker::busy() const
{
    return m_busy;
}

void ChatChecker::setStatus(const QString &value)
{
    if (m_status == value)
        return;

    m_status = value;
    emit statusChanged();
}

void ChatChecker::setLoggedIn(bool value)
{
    if (m_loggedIn == value)
        return;

    m_loggedIn = value;
    emit loggedInChanged();
}

void ChatChecker::setBusy(bool value)
{
    if (m_busy == value)
        return;

    m_busy = value;
    emit busyChanged();
}

void ChatChecker::setLastSeenId(int value)
{
    if (m_lastSeenId == value)
        return;

    m_lastSeenId = value;
    saveLastSeenIdSetting(m_lastSeenId);
    emit lastSeenIdChanged();
}

void ChatChecker::login()
{
    if (m_busy) {
        setStatus("Busy");
        return;
    }

    if (m_user.isEmpty() || m_password.isEmpty()) {
        setStatus("User and password are required");
        return;
    }

    setBusy(true);
    setStatus("Logging in...");

    QUrl url(m_baseUrl + "/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    QUrlQuery form;
    form.addQueryItem("user", m_user);
    form.addQueryItem("password", m_password);

    QNetworkReply *reply = m_network.post(request, form.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLoginReply(reply, false);
    });
}

void ChatChecker::checkNow()
{
    if (m_busy) {
        setStatus("Busy");
        return;
    }

    if (m_chat.isEmpty()) {
        setStatus("Chat is required");
        return;
    }

    setBusy(true);
    setStatus("Checking...");

    QUrl url(m_baseUrl + "/poll");
    QUrlQuery query;
    query.addQueryItem("chat", m_chat);
    query.addQueryItem("after_id", QString::number(m_lastSeenId));
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = m_network.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handlePollReply(reply);
    });
}

void ChatChecker::loginAndCheckNow()
{
    if (m_busy) {
        setStatus("Busy");
        return;
    }

    if (m_user.isEmpty() || m_password.isEmpty()) {
        setStatus("User and password are required");
        return;
    }

    if (m_chat.isEmpty()) {
        setStatus("Chat is required");
        return;
    }

    setBusy(true);
    setStatus("Logging in...");

    QUrl url(m_baseUrl + "/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    QUrlQuery form;
    form.addQueryItem("user", m_user);
    form.addQueryItem("password", m_password);

    QNetworkReply *reply = m_network.post(request, form.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleLoginReply(reply, true);
    });
}

void ChatChecker::resetSession()
{
    const QList<QNetworkCookie> currentCookies =
            m_cookieJar->cookiesForUrl(QUrl(m_baseUrl));
    QList<QNetworkCookie> clearedCookies = currentCookies;

    for (QNetworkCookie &cookie : clearedCookies) {
cookie.setExpirationDate(QDateTime::fromTime_t(0));
    }

    m_cookieJar->setCookiesFromUrl(clearedCookies, QUrl(m_baseUrl));

    setLoggedIn(false);
    setLastSeenId(0);
    setStatus("Session reset");

    if (m_pollTimer->isActive()) {
        m_pollTimer->stop();
    }
}

void ChatChecker::handleLoginReply(QNetworkReply *reply, bool thenCheck)
{
    const int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();

    if (reply->error() != QNetworkReply::NoError &&
            statusCode != 303) {
        setBusy(false);
        setLoggedIn(false);
        setStatus(QString("Login network error: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    if (statusCode == 303) {
        setLoggedIn(true);

        if (!m_pollTimer->isActive()) {
            m_pollTimer->start();
        }

        setStatus("Login OK");

        reply->deleteLater();

        if (thenCheck) {
            setBusy(false);
            checkNow();
            return;
        }

        setBusy(false);
        return;
    }

    if (statusCode == 403) {
        setBusy(false);
        setLoggedIn(false);
        setStatus(QString("Login failed: %1").arg(QString::fromUtf8(body)));
        reply->deleteLater();
        return;
    }

    setBusy(false);
    setLoggedIn(false);
    setStatus(QString("Unexpected login response: %1").arg(statusCode));
    reply->deleteLater();
}

void ChatChecker::handlePollReply(QNetworkReply *reply)
{
    const int statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        setBusy(false);
        setStatus(QString("Poll network error: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    if (statusCode == 403) {
        setBusy(false);
        setLoggedIn(false);
        setStatus("Poll denied: not logged in");
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        setBusy(false);
        setStatus(QString("Invalid JSON: %1").arg(parseError.errorString()));
        reply->deleteLater();
        return;
    }

    const QJsonObject obj = doc.object();
    const int latestMessageId = obj.value("latest_message_id").toInt(-1);

    if (latestMessageId < 0) {
        setBusy(false);
        setStatus("Missing latest_message_id");
        reply->deleteLater();
        return;
    }

    if (latestMessageId > m_lastSeenId) {
        const int previousId = m_lastSeenId;
        setLastSeenId(latestMessageId);

        if (previousId > 0) {
            emit notifyRequested("Pling",
                                 QString("New message in %1 (id %2)")
                                 .arg(m_chat)
                                 .arg(latestMessageId));
            setStatus(QString("New message detected: %1").arg(latestMessageId));
        } else {
            setStatus(QString("Baseline updated to %1").arg(latestMessageId));
        }
    } else {
        setStatus("No new messages");
    }

    setBusy(false);
    reply->deleteLater();
}
