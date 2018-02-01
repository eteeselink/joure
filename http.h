#ifndef HTTP_H
#define HTTP_H

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslConfiguration>

#include "vendor/benlau/asyncfuture.h"

/*!
 * Prepares and cleans up a HTTP request to a `path` on our
 * Firebase account.
 * In `exec`, call one of the manager HTTP verbs, e.g. `manager->get(request)`.
 * In `handle`, parse the reply and turn it into data. The reply will be cleaned
 * up afterwards.
 *
 * Returns a QFuture that resolves with the data returned from `handle`
 */
template<typename T>
QFuture<T> fetch(
        QString path,
        std::function<void (QNetworkAccessManager*, QNetworkRequest&)> exec,
        std::function<T (QNetworkReply*)> handle
    ) {
    QNetworkRequest request;

    // enable https
    QSslConfiguration ssl = QSslConfiguration::defaultConfiguration();
    ssl.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(ssl);
    request.setHeader(QNetworkRequest::ServerHeader, "application/json");

    request.setUrl(QUrl("https://joure-public.firebaseio.com" + path));

    // do the request
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    exec(manager, request);

    // connect the handler and tie its result to the returned QFuture.
    auto deferred = AsyncFuture::deferred<T>();
    QObject::connect(
        manager, &QNetworkAccessManager::finished,
        [=]( QNetworkReply* reply ) mutable {
            T result = handle(reply);
            deferred.complete(result);
            reply->deleteLater();
            manager->deleteLater();
        }
   );
   return deferred.future();

}

template<typename T>
QFuture<T> put(const QString& path, QByteArray& data, std::function<T (QNetworkReply*)> handle) {
    return fetch(
        path,
        [&](auto manager, auto request) { manager->put(request, data); },
        handle
    );
}

/*!
 *
 */
template<typename T>
QFuture<T> get(const QString& path, std::function<T (QNetworkReply*)> handle) {
    return fetch(
        path,
        [](auto manager, auto request) { manager->get(request); },
        handle
    );
}

#endif // HTTP_H
