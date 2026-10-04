# Fills in the table of contents of the test plan .docx and exports it to PDF via Word.
# Run after build-testplan.py (python-docx cannot compute page numbers itself).
# The file is located by pattern, not by name: Windows PowerShell 5.1 reads a
# BOM-less script as ANSI and would garble a Cyrillic file name written here.
$docx = Get-ChildItem -LiteralPath $PSScriptRoot -Filter '*.docx' |
    Where-Object { $_.Name -notlike '~$*' } | Select-Object -First 1
if (-not $docx) { throw "No .docx found in $PSScriptRoot - run build-testplan.py first" }
$pdf = [System.IO.Path]::ChangeExtension($docx.FullName, '.pdf')

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
try {
    $doc = $word.Documents.Open($docx.FullName)
    $doc.Fields.Update() | Out-Null
    foreach ($toc in $doc.TablesOfContents) { $toc.Update() }
    $doc.Save()
    $doc.ExportAsFixedFormat($pdf, 17)
    "pages: $($doc.ComputeStatistics(2)), words: $($doc.ComputeStatistics(0))"
    $doc.Close($false)
} finally {
    $word.Quit()
}
