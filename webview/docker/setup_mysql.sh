set -e
set -x

mysql_install_db
# Workaround mariadb package bug https://www.moucha.cc/charon/tag/mariadb/
chown -R mysql:mysql /var/lib/mysql

# Start the MySQL daemon in the background.
/usr/bin/mysqld_safe &
mysql_pid=$!

until mysqladmin ping &>/dev/null; do
  echo -n "."; sleep 0.2
done

# Remove wildcard user (fixes localhost connection)
mysql -e "DELETE FROM mysql.user WHERE User='';"
# Create cnipol MySQL user
mysql -e "CREATE USER 'cnipol'@'%' IDENTIFIED BY '(n!P0l'; GRANT SELECT, INSERT, UPDATE, DELETE ON *.* TO 'cnipol'@'%'; FLUSH PRIVILEGES;"

# Create cnipol database
mysql < database_schema.sql

# Tell the MySQL daemon to shutdown.
mysqladmin shutdown

# Wait for the MySQL daemon to exit.
wait $mysql_pid

