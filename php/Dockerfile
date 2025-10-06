# Use uma imagem base oficial com Apache e PHP
FROM php:8.2-apache

# Instale extensões necessárias (mysqli para conectar ao MySQL)
RUN docker-php-ext-install mysqli pdo_mysql

# Ative módulos Apache se necessário
RUN a2enmod rewrite

# Copie o script PHP para o diretório web
COPY post-data.php /var/www/html/

# Defina permissões
RUN chown -R www-data:www-data /var/www/html
