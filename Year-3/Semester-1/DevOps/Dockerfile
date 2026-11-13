# Статична сторінка, яку віддає nginx.
# Версія базового образу закріплена, бо тег latest рухомий і робить збірку невідтворюваною.
FROM nginx:1.30.6-alpine

LABEL org.opencontainers.image.title="DevOps Pipeline Demo" \
      org.opencontainers.image.authors="Nazar Stepanenko <Gangsterstepa@gmail.com>" \
      org.opencontainers.image.source="https://github.com/d3Par1/devops-lab1"

COPY index.html /usr/share/nginx/html/index.html

# Лише документує порт; публікується він параметром -p під час docker run.
EXPOSE 80
