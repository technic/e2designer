#!/usr/bin/python

"""Simple python script to post meta information to bintray.com"""

import os
import requests

if __name__ == "__main__":
    # Configuration comes from environment variables
    user = os.environ['BINTRAY_USER']
    key = os.environ['BINTRAY_API_KEY']
    repo = os.environ['BINTRAY_REPO']
    package = os.environ['BINTRAY_PACKAGE']

    api_url = "https://api.bintray.com"

    with open('README.md') as f:
        readme = f.read()
    print("Uploading readme...")
    r = requests.post(
        url="{api_url}/packages/{user}/{repo}/{package}/readme".format(
            api_url=api_url, user=user, repo=repo, package=package),
        json={
            "bintray": {
                "syntax": "markdown",
                "content": readme,
            }
        }, auth=(user, key))
    r.raise_for_status()
    print("Done.")
