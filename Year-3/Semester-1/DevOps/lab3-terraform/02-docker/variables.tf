variable "docker_host" {
  description = "Адреса Docker Engine; null - взяти з DOCKER_HOST або типову"
  type        = string
  default     = null
}

variable "image_repository" {
  description = "Репозиторій образу застосунку з ЛР2"
  type        = string
  default     = "ghcr.io/d3par1/hello-devops"
}

variable "app_version" {
  description = "Тег образу (версія застосунку)"
  type        = string
  default     = "2.0.1"

  validation {
    condition     = can(regex("^[0-9]+[.][0-9]+[.][0-9]+$", var.app_version))
    error_message = "app_version має бути семантичною версією, наприклад 2.0.2 (не latest)."
  }
}

variable "environments" {
  description = "Середовища: ім'я -> порт на хості та ліміт пам'яті"
  type = map(object({
    port      = number
    memory_mb = number
  }))
  default = {
    staging    = { port = 8081, memory_mb = 64 }
    production = { port = 8082, memory_mb = 64 }
  }

  validation {
    condition     = alltrue([for name, e in var.environments : can(regex("^[a-z][a-z0-9-]*$", name))])
    error_message = "Ім'я середовища: малі латинські літери, цифри й дефіс."
  }
  validation {
    condition     = alltrue([for e in values(var.environments) : e.port >= 1024 && e.port <= 65535])
    error_message = "Порт середовища має бути в межах 1024-65535 (без прав адміністратора)."
  }
  validation {
    condition     = length(distinct([for e in values(var.environments) : e.port])) == length(var.environments)
    error_message = "Порти середовищ не повинні повторюватися."
  }
}
