#!/bin/bash
root_path=$(dirname "$(readlink -f "$0")")
echo "You Can Input q For Abort."
read -p "Input Git Commit Info: " commit_info
if [ "$commit_info" != "q" ]; then
    cd "$root_path"
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
    git commit -m "$commit_info"
    git push
fi
