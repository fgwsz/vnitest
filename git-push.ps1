$root_path=Split-Path -Parent $MyInvocation.MyCommand.Definition
echo "You Can Input q For Abort."
$commit_info=Read-Host -Prompt "Input Git Commit Info"
if(!($commit_info -eq "q")){
    cd $root_path
    git add ./include/*
    git add ./single-include/*
    git add ./source/*
    git add ./tests/*
    git add ./CMakeLists.txt
    git add ./LICENSE
    git add ./*.md
    git add ./.gitignore
    git add ./*.ps1
    git add ./*.sh
    git add ./*.cmake
    git commit -m $commit_info
    git push
}
