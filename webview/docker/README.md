This assumes that you already know what Docker is, if not, you can read articles about Docker on the internet.
The purpose of the presented Dockerfile is to provide a "proper" and "convenient" container-based way to develop cnipol webview online pages. This also can potentially be used to deploy production pages as well.
The main drawback here is that RACF, for obvious reasons, will never provide an official docker support, so one has to use a machine to which they have a superuser access (for example your workstation).

== Using ==

    docker build -t webview .
    docker run -p 31337:80 -p 31338:3306 -v `pwd`:/var/www/html/cnipol webview
