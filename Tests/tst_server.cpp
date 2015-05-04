#include <QString>
#include <QtTest>
#include <QJsonObject>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>

#include "communicationhandler.h"

class ServerTest : public QObject
{
    Q_OBJECT

public:
    ServerTest();

private Q_SLOTS:
    void init();
    void cleanup();

    void testSuccessfulAuthenticationFlow();
    void testUnsuccessfulAuthFlow();

    void testSearchRequestQueryResponseMatchesQuery();
    void testSearchResponseMatchesExpectedResult();
    void testNonsenseQueryReturnsZeroResults();

    void testEnsureMediaRequestsAlwaysReturnSameFile();
    void testEnsureMediaResponseMatchesExpected();
    void testNonsenseHashReturnsEmptyByteArray();
    void testInvalidJSONDoesNothing();
private:
    CommunicationHandler *comm;
    void sendSuccessfulAuth();
};

ServerTest::ServerTest()
{

}

void ServerTest::init()
{
    comm = new CommunicationHandler("ws://postgres-migrate-test.cloudapp.net:8900");
    QSignalSpy initSpy(comm, SIGNAL(commConnected()));
    QVERIFY(initSpy.isValid());

    QTest::qWait(250);
    QCOMPARE(initSpy.count(), 1);
    sendSuccessfulAuth();
}

void ServerTest::cleanup()
{
    comm->deleteLater();
}

void ServerTest::testInvalidJSONDoesNothing()
{
    QSignalSpy nonsenseSpy(comm->socket, SIGNAL(textMessageReceived(QString)));
    QVERIFY(nonsenseSpy.isValid());
    QSignalSpy commSpy1(comm, SIGNAL(onSearchReceived(QJsonObject)));
    QSignalSpy commSpy2(comm, SIGNAL(onMediaReceived(QByteArray)));
    QSignalSpy commSpy3(comm, SIGNAL(onErrorReceived(QString)));
    QSignalSpy commSpy4(comm, SIGNAL(onAuthReceived(bool)));

    QJsonObject nonsense
    {
        {"nonsense", "this is an invalid json request"},
        {"order", ""}
    };

    comm->socket->sendTextMessage(QJsonDocument(nonsense).toJson());

    QTest::qWait(5000);
    QVERIFY(commSpy1.size() == 0);
    QVERIFY(commSpy2.size() == 0);
    QVERIFY(commSpy3.size() == 0);
    QVERIFY(commSpy4.size() == 0);
}

void ServerTest::testNonsenseHashReturnsEmptyByteArray()
{
    QSignalSpy mediaSpy(comm, SIGNAL(onMediaReceived(QByteArray)));
    QVERIFY(mediaSpy.isValid());

    comm->sendMediaRequest("now put your hands up!"); // has spaces, so guaranteed not to be valid

    QTest::qWait(7000);
    QCOMPARE(mediaSpy.count(), 1);

    QList<QVariant> ret = mediaSpy.takeFirst();
    QByteArray sent = ret.at(0).toByteArray();

    QVERIFY(sent.size() == 0);
}

void ServerTest::testEnsureMediaResponseMatchesExpected()
{
    QSignalSpy mediaSpy(comm, SIGNAL(onMediaReceived(QByteArray)));
    QVERIFY(mediaSpy.isValid());

    QFile onDisk("/home/sri/Music/Singles/flo-rida-feat-sage-the-gemini-igdfr.mp3");
    QVERIFY(onDisk.open(QIODevice::ReadOnly));

    QByteArray disk = onDisk.readAll();

    comm->sendMediaRequest("AwINBJAgoRW3OzI=");

    QTest::qWait(7000);
    QCOMPARE(mediaSpy.count(), 1);

    QList<QVariant> ret = mediaSpy.takeFirst();
    QByteArray sent = ret.at(0).toByteArray();

    QCOMPARE(sent, disk);
}

void ServerTest::testEnsureMediaRequestsAlwaysReturnSameFile()
{
    QSignalSpy mediaSpy(comm, SIGNAL(onMediaReceived(QByteArray)));
    QVERIFY(mediaSpy.isValid());

    comm->sendMediaRequest("AwINBJAgoRW3OzI=");

    QTest::qWait(7000);
    QCOMPARE(mediaSpy.count(), 1);

    QList<QVariant> ret = mediaSpy.takeFirst();
    QByteArray first = ret.at(0).toByteArray();

    comm->sendMediaRequest("AwINBJAgoRW3OzI=");

    QTest::qWait(7000);
    QCOMPARE(mediaSpy.count(), 1);

    QList<QVariant> ret2 = mediaSpy.takeFirst();
    QByteArray second = ret2.at(0).toByteArray();

    QCOMPARE(first, second);
}

void ServerTest::testNonsenseQueryReturnsZeroResults()
{
    QSignalSpy searchSpy(comm, SIGNAL(onSearchReceived(QJsonObject)));
    QVERIFY(searchSpy.isValid());

    comm->sendSearchRequest("aosenuthnetuhosentuhasorediaoseuhasoa");
    QTest::qWait(3000);

    QCOMPARE(searchSpy.count(), 1);
    QList<QVariant> args = searchSpy.takeFirst();

    QJsonObject obj = args.at(0).toJsonObject();
    QJsonObject res = obj["response"].toObject()["results"].toObject();

    QCOMPARE(res["number"].toInt(), 0);

    QJsonArray first = obj["response"]
                       .toObject()["results"]
                       .toObject()["results"]
                       .toArray();
    QCOMPARE(first.size(), 0);
}

void ServerTest::testSearchResponseMatchesExpectedResult()
{
    QSignalSpy searchSpy(comm, SIGNAL(onSearchReceived(QJsonObject)));
    QVERIFY(searchSpy.isValid());

    comm->sendSearchRequest("Flo Rida");
    QTest::qWait(3000);

    QCOMPARE(searchSpy.count(), 1);
    QList<QVariant> args = searchSpy.takeFirst();

    QJsonObject obj = args.at(0).toJsonObject();
    QJsonObject res = obj["response"].toObject()["results"].toObject();

    QCOMPARE(res["number"].toInt(), 11);
    QJsonObject test
    {
        {"hash", "AwINBJAgoRW3OzI="},
        {"order", ""}
    };

    QJsonObject metadata
    {
        {"album", "My House"},
        {"artist", "Flo Rida Feat. Sage The Gemini"},
        {"genre", "Hip-Hop, Trap"},
        {"length", "3:08"},
        {"title", "G.D.F.R. (Going Down For Real)"},
        {"track_number", 6}
    };

    test["metadata"] = metadata;

    QJsonObject first = obj["response"]
                       .toObject()["results"]
                       .toObject()["results"]
                       .toArray()
                       .at(0)
                       .toObject();

    QVERIFY(test == first);
}

void ServerTest::sendSuccessfulAuth()
{
    QSignalSpy authSpy(comm, SIGNAL(onAuthReceived(bool)));
    QVERIFY(authSpy.isValid());
    comm->sendAuthRequest("serverPass");
    QTest::qWait(500);

    QCOMPARE(authSpy.count(), 1);
    QList<QVariant> args = authSpy.takeFirst();

    QVERIFY(args.at(0).toBool() == true);
}

void ServerTest::testSearchRequestQueryResponseMatchesQuery()
{
    QSignalSpy searchSpy(comm, SIGNAL(onSearchReceived(QJsonObject)));
    QVERIFY(searchSpy.isValid());

    QString queryStr = "Flo Rida";

    comm->sendSearchRequest(queryStr);
    QTest::qWait(3000);

    QCOMPARE(searchSpy.count(), 1);
    QList<QVariant> args = searchSpy.takeFirst();

    QJsonObject obj = args.at(0).toJsonObject();

    QJsonObject res = obj["response"].toObject();

    QCOMPARE(res["query"].toString(), queryStr);
}

void ServerTest::testSuccessfulAuthenticationFlow()
{
    QSignalSpy spy(comm, SIGNAL(onAuthReceived(bool)));
    QVERIFY(spy.isValid());

    comm->sendAuthRequest("serverPass");

    QTest::qWait(250);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();

    QVERIFY(args.at(0).toBool() == true);
}

void ServerTest::testUnsuccessfulAuthFlow()
{
    QSignalSpy spy(comm, SIGNAL(onAuthReceived(bool)));
    QSignalSpy disconnect(comm, SIGNAL(connectionLost()));
    QVERIFY(spy.isValid());
    QVERIFY(disconnect.isValid());

    comm->sendAuthRequest("definitelyNotThePassword");

    QTest::qWait(250);

    QCOMPARE(spy.count(), 0);

    QTest::qWait(5000);
    QList<QVariant> args = spy.takeFirst();

    QVERIFY(args.at(0).toBool() == false);
    QVERIFY(disconnect.count() > 0);
}

QTEST_GUILESS_MAIN(ServerTest)

#include "tst_server.moc"
