output "urls" {
  description = "Адреси середовищ"
  value       = { for name, e in var.environments : name => "http://localhost:${e.port}" }
}

output "image" {
  description = "Образ, з якого запущено контейнери"
  value       = "${local.image_ref} (${data.docker_registry_image.app.sha256_digest})"
}

output "containers" {
  description = "Ідентифікатори контейнерів"
  value       = { for name, c in docker_container.web : name => substr(c.id, 0, 12) }
}
