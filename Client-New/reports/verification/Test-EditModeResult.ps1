# PowerShell 7. Reads NUnit 3 evidence only; never launches Unity or modifies the project.
[CmdletBinding()]
param([Parameter(Mandatory)][string]$ResultPath)
$ErrorActionPreference = 'Stop'
$record = [ordered]@{ Verified = $false; ResultPath = $ResultPath; Total = 0; Passed = 0; Failed = 0; Reason = '' }
try {
    if (-not (Test-Path -LiteralPath $ResultPath -PathType Leaf)) { throw 'Result XML is missing; Unity test execution is unproven.' }
    $settings = [System.Xml.XmlReaderSettings]::new()
    $settings.DtdProcessing = [System.Xml.DtdProcessing]::Prohibit
    $settings.XmlResolver = $null
    $xmlReader = [System.Xml.XmlReader]::Create((Resolve-Path -LiteralPath $ResultPath).Path, $settings)
    try {
        $document = [System.Xml.XmlDocument]::new()
        $document.XmlResolver = $null
        $document.Load($xmlReader)
    } finally { $xmlReader.Dispose() }
    $run = $document.DocumentElement
    if ($run.Name -ne 'test-run') { throw 'Expected NUnit 3 test-run root.' }
    foreach ($attribute in @('total', 'passed', 'failed', 'skipped')) {
        $value = 0
        if (-not [int]::TryParse($run.GetAttribute($attribute), [ref]$value) -or $value -lt 0) { throw "Invalid $attribute count." }
    }
    $record.Total = [int]$run.GetAttribute('total')
    $record.Passed = [int]$run.GetAttribute('passed')
    $record.Failed = [int]$run.GetAttribute('failed')
    $cases = @($document.SelectNodes('//test-case'))
    $passedCases = @($cases | Where-Object { $_.GetAttribute('result') -eq 'Passed' }).Count
    if ($run.GetAttribute('result') -ne 'Passed' -or $record.Total -le 0 -or $record.Failed -ne 0 -or $record.Passed -ne $record.Total -or [int]$run.GetAttribute('skipped') -ne 0 -or $cases.Count -ne $record.Total -or $passedCases -ne $record.Total) {
        throw 'Tests are empty, failed, skipped, or counts disagree with individual cases.'
    }
    $record.Verified = $true
    $record.Reason = 'Non-empty NUnit result has only passing cases. Verify Unity command, exit code, project and timestamp separately.'
} catch { $record.Reason = $_.Exception.Message }
$record | ConvertTo-Json -Compress
if ($record.Verified) { exit 0 } else { exit 2 }
