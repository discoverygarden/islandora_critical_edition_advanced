BUILD STATUS
------------
Current build status:
[![Build Status](https://travis-ci.org/discoverygarden/islandora_critical_edition_advanced.png?branch=7.x)](https://travis-ci.org/discoverygarden/islandora_critical_edition_advanced)

CONTENTS OF THIS FILE
---------------------

 * summary
 * configuration

SUMMARY
-------

A module for creating advanced critical editions of Islandora objects using the
advanced data model


CONFIGURATION
--------------

This module requires the Islandora Critical Edition Solution Pack.
The saxon.jar file contained in the dependencies directory must by moved or
copied to the webapps directory of the tomcat server.

This will normally be /usr/local/fedora/tomcat/webapps/

INSTALL MONGO
--------------

First install Mongo, then:

```bash
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 7F0CEB10
echo 'deb http://downloads-distro.mongodb.org/repo/ubuntu-upstart dist 10gen' | sudo tee /etc/apt/sources.list.d/10gen.list
sudo apt-get update
sudo mkdir –p /data/db
chown -R vagrant /data
```

Run from Mongo prompt:

```
db.addUser('admin', 'jabberw0cky', 'userAdminAnyDatabase')
```

Then from the command line:

```bash
sudo pecl install mongo
```

Add then following line to your php.ini file /etc/php5/apache2/:

```
extension=mongo.so
```

When finished just restart apache service.
