Get-ChildItem -Path $RootDirectory\streamlabs-build\distribute\obs-studio-node -Filter *.pdb -File | ForEach-Object {&"$RootDirectory\dump_syms.exe" $_.FullName > $RootDirectory\syms\$_} 
Get-ChildItem -Path $RootDirectory\streamlabs-build\distribute\obs-studio-node -Filter *.pdb -Recurse -File | ForEach-Object {&"$RootDirectory\dump_syms.exe" $_.FullName > $RootDirectory\syms\$_}
Get-ChildItem -Path $RootDirectory\syms\*.pdb | Rename-Item -NewName { $_.Name -Replace ".pdb",".sym"}
Get-ChildItem -Path $RootDirectory\syms -Filter *.sym -File | ForEach-Object {Get-Content $_.FullName | Out-File -Encoding Ascii "$RootDirectory\syms\ascii\$($_.BaseName).sym" } 

Move-Item -Path  $RootDirectory\syms\ascii\obs_studio_client.sym -Destination $RootDirectory\syms\ascii\client
Move-Item -Path  $RootDirectory\syms\ascii\obs-frontend-api.sym -Destination $RootDirectory\syms\ascii\client

.\sentry-cli.exe --auth-token "d6526d57bb84421eaaeff7983639897de9a0c51ab5274cf4b89d3ad3944d3cbd" upload-dif --org streamlabs-obs --project obs-server "$RootDirectory\syms\ascii"
.\sentry-cli.exe --auth-token "d6526d57bb84421eaaeff7983639897de9a0c51ab5274cf4b89d3ad3944d3cbd" upload-dif --org streamlabs-obs --project obs-server-preview "$RootDirectory\syms\ascii"
.\sentry-cli.exe --auth-token "d6526d57bb84421eaaeff7983639897de9a0c51ab5274cf4b89d3ad3944d3cbd" upload-dif --org streamlabs-obs --project obs-client "$RootDirectory\syms\ascii\client"
