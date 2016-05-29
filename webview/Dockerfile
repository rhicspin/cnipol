
FROM centos/httpd

# Set proper timezone
RUN ln -sf ../usr/share/zoneinfo/America/New_York /etc/localtime

# Install Apache, MySQL, PHP
RUN yum -y install mariadb-server php php-mysql

# Install supervisord
RUN yum -y install epel-release
RUN yum -y install python-setuptools
RUN easy_install supervisor

# Deploy webview
RUN sed --in-place 's/short_open_tag = Off/short_open_tag = On/' /etc/php.ini
COPY  ./ /var/www/html/cnipol/
COPY docker/setup_mysql.sh .
COPY docker/database_schema.sql .
RUN ./setup_mysql.sh

EXPOSE 80 3306

COPY docker/supervisord.conf /etc/
CMD ["supervisord"]
