#include <QString>
#include <QtTest>
#include <QJsonObject>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "schandler.h"
#include "communicationhandler.h"

#include "databasehandler.h"
#include "JSONhandler.h"
#include "songhandler.h"
#include "playlisthandler.h"

#include <libspotify/api.h>
#include "Spotify/qtspotifysession.h"
#include "Spotify/qtspotifywrapper.h"
#include "Settings/settings_storage.h"



class ServerTest : public QObject
{
    Q_OBJECT

public:
    ServerTest();
    bool FLAG_SPOTIFY_READY_FOR_TEST;
    QtSpotifySession *spotifySessionSingleton;

private Q_SLOTS:
    void init();
    void initTestCase();
    void cleanup();

    void testSuccessfulAuthenticationFlow();
    void testUnsuccessfulAuthFlow();

    void testSearchRequestQueryResponseMatchesQuery();
    void testSearchResponseMatchesExpectedResult();
    void testNonsenseQueryReturnsZeroResults();

    void testSCSuccessfulQuery();
    void testSCUnsuccessfulQuery();
    void testSCSuccessfulSearch();
    void testSCUnsuccessfulSearch();
    void testSCSuccessfulRequest();
    void testSCUnsuccessfulRequest();
    void testSCSuccessfulFormat();
    void testSCUnuccessfulFormat();

    void dbMetaDataTest();
    void dbIsMusicTest();
    void dbOpenTest();
    void dbCreateTableTest();
    void dbPopulateQueryTest();
    void dbGetSongTest();
    void dbGenerateJSONResult();
    void getSongAsByteArrayTest();
    void playlistGetSetTest();

    void testEnsureMediaRequestsAlwaysReturnSameFile();
    void testEnsureMediaResponseMatchesExpected();
    void testNonsenseHashReturnsEmptyByteArray();
    void testInvalidJSONDoesNothing();

    void spotifyInitSessionAndLogin();
    void spotifySearchResultsReceived();
    void spotifySearchResultsFormatted();


private:
    CommunicationHandler *comm;
    void sendSuccessfulAuth();
    SCHandler *sc;
    QString nothing;
    DatabaseHandler *db;
    JSONHandler *json;
    songHandler *song;
    playListHandler *playlist;
};

ServerTest::ServerTest()
{

}

void ServerTest::initTestCase()
{
    QCoreApplication::instance()->setOrganizationName("AlluvialAlphaBuild");
    QCoreApplication::instance()->setApplicationName("Alluvial");
    FLAG_SPOTIFY_READY_FOR_TEST = false;
    spotifySessionSingleton = new QtSpotifySession();
    //spotifySessionSingleton->initSpotify();
    //QtSpotifyWrapper *spotifyWrapperSingleton = new QtSpotifyWrapper();
}

void ServerTest::init()
{
    comm = new CommunicationHandler("ws://postgres-migrate-test.cloudapp.net:8900");
    QSignalSpy initSpy(comm, SIGNAL(commConnected()));
    QVERIFY(initSpy.isValid());
    sc = new SCHandler();

    db = new DatabaseHandler();
    json = new JSONHandler();
    song = new songHandler();

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

void ServerTest::testSCSuccessfulSearch(){
    QSignalSpy spy(sc, SIGNAL(onSearchComplete(QJsonArray*)));
    QVERIFY(spy.isValid());
    QJsonArray testArr = sc->search("foo fighters");
    QTest::qWait(250);
    QCOMPARE(spy.count(), 1);
    QTest::qWait(5000);
    QVERIFY(testArr.at(0).toObject()["metadata"].toObject()["artist"].toString().compare("whitepandamusic") == 0);

}

void ServerTest::testSCUnsuccessfulSearch(){
    QSignalSpy spy(sc, SIGNAL(onSearchComplete(QJsonArray*)));
    QVERIFY(spy.isValid());
    QJsonArray testArr = sc->search(nothing);
    QTest::qWait(250);
    QCOMPARE(spy.count(), 1);
    QVERIFY(testArr.isEmpty() == true);
}

void ServerTest::testSCSuccessfulRequest(){
    QByteArray testBarry = sc->request_song("https://api.soundcloud.com/tracks/102131015/download");
    QTest::qWait(5000);
    QVERIFY(testBarry.isEmpty() == false);
}

void ServerTest::testSCUnsuccessfulRequest(){
    QByteArray testBarry = sc->request_song(nothing);
    QVERIFY(testBarry.isEmpty() == true);
}

void ServerTest::testSCSuccessfulFormat(){
    QJsonArray testArr = sc->search("foo fighters");
    QJsonObject testobj = testArr.at(0).toObject();
    QJsonObject testobj_meta = testobj["metadata"].toObject();
    QTest::qWait(5000);
    QVERIFY(testArr.isEmpty() == false);

    //build expected jsonobj
    QJsonObject media{
        {"hash","https://api.soundcloud.com/tracks/102131015/download"},
        {"order",""}
    };

    QJsonObject meta{
        {"title", "Best Of Titanium (Foo Fighters // David Guetta)"},
        {"album",""}, //nope because soundcloud
        {"artist", "whitepandamusic"},
        {"track_number", 0},
        {"length", "03:30"}, //get from fucking duration
        {"genre", "Mashup"}
    };


    QJsonObject media_actual{
        {"hash",testobj["hash"].toString()},
        {"order",testobj["order"].toString()}
    };

    QJsonObject meta_actual{
        {"title", testobj_meta["title"].toString()},
        {"album",testobj_meta["album"].toString()}, //nope because soundcloud
        {"artist", testobj_meta["artist"].toString()},
        {"track_number", testobj_meta["track_number"].toString()},
        {"length", testobj_meta["length"].toString()}, //get from fucking duration
        {"genre", testobj_meta["genre"].toString()}
    };


    media["metadata"] = meta;
    media_actual["metadata"] = meta_actual;
//    qDebug() << testArr.at(0).toObject();
//    qDebug() << media;
    QCOMPARE(testArr.at(0).toObject(), media);
}

void ServerTest::testSCUnuccessfulFormat(){
    QJsonArray testArr = sc->search(nothing);
    QJsonObject testobj = testArr.at(0).toObject();
    QJsonObject testobj_meta = testobj["metadata"].toObject();
    QTest::qWait(5000);
    QVERIFY(testArr.isEmpty() == true);

    //build expected jsonobj
    QJsonObject media{
        {"hash","https://api.soundcloud.com/tracks/102131015/download"},
        {"order",""}
    };

    QJsonObject meta{
        {"title", "Best Of Titanium (Foo Fighters // David Guetta)"},
        {"album",""}, //nope because soundcloud
        {"artist", "whitepandamusic"},
        {"track_number", 0},
        {"length", "03:30"}, //get from fucking duration
        {"genre", "Mashup"}
    };


    QJsonObject media_actual{
        {"hash",testobj["hash"].toString()},
        {"order",testobj["order"].toString()}
    };

    QJsonObject meta_actual{
        {"title", testobj_meta["title"].toString()},
        {"album",testobj_meta["album"].toString()}, //nope because soundcloud
        {"artist", testobj_meta["artist"].toString()},
        {"track_number", testobj_meta["track_number"].toString()},
        {"length", testobj_meta["length"].toString()}, //get from fucking duration
        {"genre", testobj_meta["genre"].toString()}
    };


    media["metadata"] = meta;
    media_actual["metadata"] = meta_actual;
//    qDebug() << testArr.at(0).toObject();
//    qDebug() << media;
    QEXPECT_FAIL("", "Should fail due to null search", Continue);
    QCOMPARE(testArr.at(0).toObject(), media);
}

void ServerTest::testSCSuccessfulQuery(){
    int count = sc->query("foo fighters", "q");
    QTest::qWait(500);
    QVERIFY(count > 1);
}

void ServerTest::testSCUnsuccessfulQuery(){
    int count = sc->query(nothing, "q");
    QTest::qWait(500);
    QVERIFY(count < 1);
}

void ServerTest::dbMetaDataTest(){
    QString testSong = "./dbTest/01 - Bawitdaba.mp3";
    MD result;
    result = db->ripMetaData(testSong);
    MD test;
    test.title = "Bawitdaba";
    test.artist = "Kid Rock";
    test.album = "Devil Without A Cause";
    test.genre = "Rock";
    //TODO: change this to wherever you saved the dbTest file with test song
    test.filepath = "./dbTest/01 - Bawitdaba.mp3";
    test.PKID = 2003;
    test.track_num = 1;
    test.length_min = 4;
    test.length_sec = 25;

    QVERIFY(test.title == result.title);
    QVERIFY(test.artist == result.artist);
    QVERIFY(test.album == result.album);
    QVERIFY(test.genre == result.genre);
    QVERIFY(test.filepath == result.filepath);
    QVERIFY(test.PKID == result.PKID);
    QVERIFY(test.track_num == result.track_num);
    QVERIFY(test.length_min == result.length_min);
    QVERIFY(test.length_sec == result.length_sec);


}

void ServerTest::dbIsMusicTest(){
    //TODO: change this to wherever you saved the dbTest file with test song
    bool result = db->isMusic("./dbTest/01 - Bawitdaba.mp3");
    QVERIFY(result == true);
}

void ServerTest::dbOpenTest(){
    bool result = db->openDB();
    QVERIFY(result == true);
}

void ServerTest::dbCreateTableTest(){
    db->openDB();
    bool result =  db->createTable();

    QVERIFY(result == true);
}

void ServerTest::dbPopulateQueryTest(){
    QString test = "Kid Rock";
    //TODO: change this to wherever you saved the dbTest file with test song
    qDebug() << QDir::currentPath();
    QDir dir("/home/sri/Documents/Capstone/build-Alluvial-Desktop-Debug/Tests/dbTest");
    std::vector <MD> MDresult;
    QString result;
    db->openDB();
    db->DBpopulate(dir);
    MDresult = db->queryDB("Kid Rock");
    result = MDresult.at(0).artist;
    QVERIFY(test == result);

}

void ServerTest::dbGetSongTest(){
    //TODO Change to one, your db will have only one entry
    int PKID = 1;
    //TODO: change this to wherever you saved the dbTest file with test song
    QString test = "/home/sri/Documents/Capstone/build-Alluvial-Desktop-Debug/Tests/dbTest/01 - Bawitdaba.mp3";
    QString result = db->getSongFP(PKID);
    QVERIFY(test == result);

    db->deleteLater();
    QTest::qWait(2000);
}

void ServerTest::dbGenerateJSONResult(){

    QString test = "Kid Rock";
    QJsonArray *testArray = new QJsonArray();
    QJsonArray *result = new QJsonArray();
    result = json->generateResults(test);
    QJsonObject media{
        {"hash", 1},
        {"order",""},
    };
    QJsonObject meta{
        {"title", "Bawitdaba"},
        {"album", "Devil Without A Cause"},
        {"artist", "Kid Rock"},
        {"track_number", 1},
        {"length", "4:25"},
        {"genre", "Rock"}
    };
    media["metadata"] = meta;
    testArray->append(media);
    QCOMPARE(result->at(0).toObject(), testArray->at(0).toObject());

}

void ServerTest::getSongAsByteArrayTest(){
    //TODO Change to one, your db will have only one entry
    QByteArray result;
    int PKID = 1;
    QFile file("./dbTest/01 - Bawitdaba.mp3");
    file.open(QIODevice::ReadOnly);
    QByteArray test = file.readAll();
    result = song->getSong(PKID);
    QCOMPARE(result, test);
}

void ServerTest::playlistGetSetTest(){

    QJsonObject media{
        {"name", "PLTest"},
        {"hash", 33},
        {"order",""},
    };
    QJsonObject meta{
        {"title", "Bawitdaba"},
        {"album", "Devil Without A Cause"},
        {"artist", "Kid Rock"},
        {"track_number", 1},
        {"length", "4:25"},
        {"genre", "Rock"}
    };
    media["metadata"] = meta;

    playlist->setPlaylist(media);
    QJsonObject result = playlist->getPlaylist("PLTest");

    QCOMPARE(result, media);
}

void ServerTest::spotifyInitSessionAndLogin()
{
    QVERIFY(spotifySessionSingleton->initSpotify());
}

void ServerTest::spotifySearchResultsReceived()
{
    spotifySessionSingleton->startSearch("TEST STRING ASDFGHJKLASDFGHJKLASDFGHJKL");
    QSignalSpy searchResultsReadySpy(spotifySessionSingleton, &QtSpotifySession::searchResultReady);

    QVERIFY(searchResultsReadySpy.isValid());

    QTest::qWait(2000);     //making the Test wait while the search is being executed by libSpotify
    QVERIFY(!searchResultsReadySpy.isEmpty());
}
void ServerTest::spotifySearchResultsFormatted()
{
    spotifySessionSingleton->startSearch("TEST STRING ASDFGHJKLASDFGHJKLASDFGHJKL");
    QSignalSpy searchResultsReadySpy(spotifySessionSingleton, &QtSpotifySession::searchResultReady);
    QTest::qWait(2000);     //making the Test wait while the search is being executed by libSpotify
    QVERIFY(!searchResultsReadySpy.isEmpty());
}

QTEST_GUILESS_MAIN(ServerTest)

#include "tst_server.moc"
