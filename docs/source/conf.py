import os
import sys

project = 'F103_2025E_Gimbal'
copyright = '2026, kaouduo'
author = 'kaouduo'

# -- General configuration ---------------------------------------------------

language = 'zh_CN'

extensions = [
    'breathe',
    'sphinx_rtd_theme',
    'myst_parser',
]

# -- MyST configuration ------------------------------------------------------
myst_enable_extensions = [
    "colon_fence",
]

# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

html_theme_options = {
    'logo_only': False,
    'display_version': True,
    'prev_next_buttons_location': 'bottom',
    'style_external_links': False,
    'vcs_pageview_mode': '',
    'style_nav_header_background': '#2980B9', # Customize header color
    'collapse_navigation': True,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'includehidden': True,
    'titles_only': False
}

# -- Breathe configuration ---------------------------------------------------

breathe_projects = {
    "MyProject": "../build/doxygen/xml"
}
breathe_default_project = "MyProject"
