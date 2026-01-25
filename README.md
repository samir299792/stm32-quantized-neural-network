Fork this repo to your Dartmouth GitHub account, then clone your copy of the repo to your ENGS-62 workspace on your personal computer.  

***Be sure you are cloning your copy of the repo, and not the main course repo! You will not be able to save/submit your work if you clone the main course repo:***

OSX: open terminal and change into your ENGS-62-workspace folder (mine is in my Documents folder under ENGS-62-workspace)

Win: open "git bash" and change into your ENGS-62-workspace folder 
```
cd ~/Documents/ENGS-62-workspace/
git clone git@github.com:<githubid>/W26-W3-LA.git
```
Note that this lab does not require python.  You may skip the python intialization steps, but it does no harm to perform them if you do.

OSX:
```
cd W26-W3-LA
/usr/local/bin/python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

Win:
```
cd W26-W3-LA
python -m venv .venv
. .venv/Scripts/activate
pip install -r requirements.txt
```

You can now launch VSCode and open the W26-W3-LA folder.  

Be sure to install any recommended VSCode extensions (a recommendations box will pop up within the first 30-60 seconds after opening VSCode if any extensions are recommended).

Begin by opening the assignment.ipynb file
