Start-Process -FilePath "Build\x64\Release\bin\server.exe" -ArgumentList "0.0.0.0 8033 10"

for ($i = 1; $i -le 10; $i++) {
    Start-Process -FilePath "Build\x64\Release\bin\client.exe" -ArgumentList "127.0.0.1 8033"
}