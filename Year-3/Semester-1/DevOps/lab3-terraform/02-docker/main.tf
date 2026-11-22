locals {
  image_ref = "${var.image_repository}:${var.app_version}"
  labels = {
    "com.kpi.course" = "devops"
    "com.kpi.lab"    = "lr3"
  }
}

# Тег можна перепризначити, дайджест - ні. Читаємо дайджест з реєстру під час plan
# і запускаємо образ саме за ним.
data "docker_registry_image" "app" {
  name = local.image_ref
}

resource "docker_image" "app" {
  name          = "${var.image_repository}@${data.docker_registry_image.app.sha256_digest}"
  keep_locally  = true
  pull_triggers = [data.docker_registry_image.app.sha256_digest]
}

resource "docker_network" "app" {
  name = "lab3-hello"
  dynamic "labels" {
    for_each = local.labels
    content {
      label = labels.key
      value = labels.value
    }
  }
}

resource "docker_container" "web" {
  for_each = var.environments

  name    = "lab3-${each.key}"
  image   = docker_image.app.image_id
  env     = ["APP_ENV=${each.key}"]
  restart = "unless-stopped"
  memory  = each.value.memory_mb
  # Без цього Docker сам ставить swap = 2 x memory, і кожен plan показує зміну.
  memory_swap = each.value.memory_mb

  ports {
    internal = 80
    external = each.value.port
    ip       = "127.0.0.1"
  }

  networks_advanced {
    name    = docker_network.app.id
    aliases = [each.key]
  }

  # Ті самі обмеження, що в ЛР2: без capabilities, без підвищення прав,
  # коренева ФС лише для читання, запис тільки в tmpfs. tmpfs за замовчуванням
  # належить root (0755), а nginx працює від UID 101, тому власника задаємо явно.
  capabilities {
    drop = ["ALL"]
  }
  security_opts = ["no-new-privileges:true"]
  read_only     = true
  tmpfs = {
    "/tmp"              = "rw,size=1m,uid=101,gid=101"
    "/var/cache/nginx"  = "rw,size=8m,uid=101,gid=101"
    "/etc/nginx/conf.d" = "rw,size=1m,uid=101,gid=101"
  }

  # HEALTHCHECK описано в образі; wait = true чекає стану healthy.
  wait         = true
  wait_timeout = 60

  dynamic "labels" {
    for_each = merge(local.labels, { "com.kpi.env" = each.key })
    content {
      label = labels.key
      value = labels.value
    }
  }
}

# Опис розгорнутих середовищ, згенерований з шаблону.
resource "local_file" "inventory" {
  filename        = "${path.module}/out/environments.md"
  file_permission = "0644"
  content = templatefile("${path.module}/inventory.tftpl", {
    image  = local.image_ref
    digest = data.docker_registry_image.app.sha256_digest
    envs = {
      for name, c in docker_container.web : name => {
        url       = "http://localhost:${var.environments[name].port}"
        container = c.name
        id        = substr(c.id, 0, 12)
      }
    }
  })
}
