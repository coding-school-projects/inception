# Use VM Alpine version as base image
FROM alpine:3.19.4

# Install NGINX and SSL
RUN apk update && \
	apk add --no-cache \
    nginx \
    openssl

# Expose port for HTTPS
EXPOSE 443

# Create folders for certificates
RUN mkdir -p /run/nginx /etc/nginx/ssl

# Generate SSL cert & key
RUN openssl req -newkey rsa:2048 -x509 -sha256 -days 365 -nodes \
    -out /etc/nginx/ssl/inception.crt \
    -keyout /etc/nginx/ssl/inception.key \
    -subj "/C=FI/ST=Helsinki/L=Helsinki/O=42/OU=Hive/CN=${DOMAIN_NAME}"

# Add a new user within the container
RUN adduser -D -H -s /sbin/nologin -g www-data -G www-data www-data

# Copy NGINX configuration to the container
COPY ./conf/nginx.conf /etc/nginx/nginx.conf
RUN chmod 644 /etc/nginx/nginx.conf


# Start NGINX
CMD ["nginx", "-c", "/etc/nginx/nginx.conf", "-g", "daemon off;"]