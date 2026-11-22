terraform {
  required_version = ">= 1.9"

  required_providers {
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 4.6.0"
    }
    local = {
      source  = "hashicorp/local"
      version = "~> 2.9"
    }
  }
}

# Адресу Docker беремо зі змінної оточення DOCKER_HOST, якщо змінну Terraform не задано:
# на Linux це unix-сокет, на Windows з Docker Desktop - npipe:////./pipe/docker_engine.
provider "docker" {
  host = var.docker_host
}
