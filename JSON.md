# JSON Format

Alluvial uses JSON to transmit information between servers and clients. Typically there are four kinds of messages passed between servers and clients:

- Authentication
- Media Requests
- Searches
- Playlists

## General Request Structure

In general, requests follow this general structure:
```
{
   "request_type" : "", // the type of request
    "request": {
        // request data goes here
    }
}
```
The request type is one of either `authentication`, `search`, or `media`, corresponding to each of the three types of requests. 

Responses follow a similar pattern:
```
{
    "response_type": "", // the response type
    "response": {
        // response data goes here
    }
}
```
The request and response type should be the same. This should help the client perform sanity checks. However, it should be noted that since these JSON messages and responses essentially serialize state changes it should not really matter in what order messages are received, and that there is no requirement server-side that responses are sent in the order the requests were received. Clients should be designed with this in mind.

## JSON Objects

### Media objects
Media objects are objects that represent individual media tracks. Essentially, media objects consist of a track's metadata and its unique hash. A given media object may not represent the full range of tags attached to the track, so a media object's metadata should not be taken as a canonical source of metadata. However, as the server gains support for more tag types, it shall send that data along to the client, so in the future it may become a canonical source of supported metadata. 

Right now, the list of supported metadata tags is:
- Title
- Album
- Artist
- Track number
- Length
- Genre

A media object looks something like this:
```
{
    "media": {
        "metadata": {
            "title": "Burn",
            "album": "Halcyon Days",
            "artist": "Ellie Goulding",
            "track_number": "19",
            "length": "3:51",
            "genre": "Pop"
        },
        "hash": "iobruch827alehif274iulgcdxk73alcgdju7923afgieu7",
        "order": "" // blank unless media objects is part of playlist
        "sources": [
            "local",
            "spotify"
        ]
    }
}
```
The hash is meant to play the role of UUID, ie, something that can uniquely identify a particular media file. Hashes are used to identify particular media files to the server, so they should be treated appropriately client-side. The `sources` array is meant to identify which media source(s) the file came from. If there is more than one source, this parameter will be an array, as shown in the example, otherwise it will just be a single String. 

The server will try its best to figure out if media retrieved via a streaming media source is the same as media stored locally; however, it does this entirely based on tag data, thus implicity relying on tag data being correct. This is an unfortunate shortcoming that can't really be fixed without doing manual waveform comparisons of each particular media file, which would be very expensive to compute in a resonable amonut of time. 

### Album objects
Album objects represent a single album. Albums contain metadata about the album, and a list of media objects corresponding to each track on the album. An example album object would look like this:

```
{
    "album": {
        "title": "Azure - Single",
        "artist": "Coyote Kisses",
        "year": "2014",
        "num_tracks": "1",
        "tracks": [
            // array of media objects corresponding to each track here
        ]
    }
}
```
The notable lack of album art is to reduce complexity and storage needs server-side. Clients should query the internet for album art if it is desired, or request a song from the album and scan its metadata for attached album art. Normally we would not impose such a requirement on the client, but since Alluvial clients are by nature highly dependent on having access either to the server and/or the Internet in general, this seems acceptable. 

### Playlist objects
Playlists are, at their core, lists of media objects. In addition to this list, a playlist has a name and a "last saved" field, which corresponds to the last time this playlist was saved. This field is never touched by the server. Playlists should be entirely managed by the client. A playlist object looks like this:
```
{
    "playlist": {
        "name": "My Super Cool Ellie Goulding Playlist",
        "last_saved": "2015-03-18T16:21:27.277Z",
        "tracks": [
            // array of playlist media objects here
        ]
    }
}
```
When media objects are in a playlist, the client should set the "order" field of each media object and use it to store the order of each song in the playlist. Playlist names should be unique. 

### Settings objects
Settings objects contain all the data that the user wants Alluvial to save persistently.  These settings will be saved on the server for a multitude of reasons.  The primary reason being that the server needs to communicate with third-party sources which requires the user to input username/passwords for those third-party sources.
The Settings object will contain a "last saved" field.  It will also conatin a list which has objects representing third-party  music sources.  These object will have two keys, "username" and "password" which hold the credentials of the third-party source they are children of.  Lastly,is a list called "other_settings" representing all the other settings that aren't usernames or passwords.
'''
{
    "settings":{
        "last_saved": "2015-03-18T16:21:27.277Z"
        "credentials": [
            "Spotify": {
                "username": defaultUsername
                "password": **********
            },
            SoundCloud: {
                "username": "username"
                "password": "password"
            },
            //other third-party sources
        ]
        "other_settings"[
            //a list of every other setting that the user specifiies
        ]
}
'''

## Message types

### Authentication

Clients must authenticate to the server before they are allowed to communicate further. If the client sends invalid login information, the server will immediately send an error message and terminate the connection. Authentication persists for the duration of the connectiqn. Clients will have to re-authenticate if their connection is terminated. 

The format of an authentication request sent by the client is as follows:
```
{
    "request_type": "authentication",
    "request": {
        "password": "1m501337",
    }
}
```
Upon receipt of a valid authentication request, the server will send back a response that looks like this:

```
{
    "response_type": "authentication",
    "response": {
        "success": true
    }
}
```
Passwords are, regrettably, transmitted in cleartext for the time being. In the future we plan on having a self-signed certificate generated during the installation process server-side and using secure SSL-encrypted websockets. For now, just don't re-use passwords, and make sure that your password is strong.

The value of the `success` variable will differ depending on whether the authentication request was successful. If it was, the socket will be held open and further communication will be allowed. If authentication was not successful, the server will hold the socket open for five seconds while not responding to any further communication from the client, then close the socket on its end after sending this message: 

```
{
    "response_type": "authentication",
    "response": {
        "success": false
    }
}
```

The five-second delay is to impair a potential attacker's ability to brute-force the password request in a reasonable amount of time.

### Media Requests
Media requests are requests to the server to ask for a specific media file. Upon receipt of a media request, the server will send the file as a binary message. A media request looks like this:
```
{
    "request_type": "media",
    "request": {
        "hash": "iobruch827alehif274iulgcdxk73alcgdju7923afgieu7"
    }
}
```
The server will send a binary file as a response, hence there is no particular need for a confirmation message from the server prior to file transfer. 

### Searches
When the client executes a search request, the server sends back a set of search results, grouped into the various categories of result supported. Namely, these are the top 5 or 10 song matches, the top album results, and any playlists containing that song. The structure of a search request is as follows:
```
{
    "request_type": "search",
    "request": {
        "query": "" // query goes here
    }
}
```

Right now, Alluvial does not support any specialized search keywords. This is probably going to happen in the future, so keep your eyes peeled. For now, the server naively executes a search request using the query transmitted by the client to all media sources. Search responses look something like this:

```
{
    "response_type": "search",
    "query": "Ellie Goulding" // the query string
    "results": {
        "media": {
            "number": "5",
            "results": [
                // media objects in an array here
            ]
        },
        "albums": {
            "number": "1",
            "results": [
                // album objects in an array here
            ]
        },
        "playlists": {
            "number": "3",
            "results": [
                // playlist objects in an array here
            ]
        }
    }
}
```

### Playlists
Alluvial has first-class support for playlists, including server-side storage and the flexibility to allow clients to transparently save playlists without any manual user intervention. Playlists are transmitted from the client to the server using something like this:
```
{
    "request_type": "playlist",
    "request": {
        // playlist object goes here
    }
}
```

If the save was successful, the server will send a message that looks like this: 
```
{
    "response_type": "playlist",
    "response": "OK"
}
```

If the save was unsuccessful in some way, the server will send a message that looks like this:
```
{
    "response_type": "playlist",
    "response": "Error",
    "error": "" // error message goes here
}
```
