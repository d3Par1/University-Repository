# build-docx.ps1 — конвертує zvit-temp.html у Звіт_ПР6.docx через Word COM
$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Definition
$htmlPath = Join-Path $here 'zvit-temp.html'
$docxPath = Join-Path $here 'Звіт_ПР6.docx'

if (-not (Test-Path $htmlPath)) {
    Write-Error "HTML не знайдено: $htmlPath"
    exit 1
}

Write-Output "Запуск Word COM..."
$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0

try {
    Write-Output "Відкриваю HTML..."
    $doc = $word.Documents.Open($htmlPath, $false, $true)

    Write-Output "Зберігаю як .docx..."
    $doc.SaveAs2($docxPath, 16)   # 16 = wdFormatDocumentDefault
    $doc.Close($false)
    Write-Output "✓ Створено: $docxPath"

    $size = (Get-Item $docxPath).Length
    Write-Output ("Розмір файлу: {0:N0} байт ({1:N1} KB)" -f $size, ($size / 1024))
}
finally {
    $word.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
}
