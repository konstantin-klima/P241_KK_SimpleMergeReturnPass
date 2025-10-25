#!/bin/bash

pdflatex -output-directory=.. -jobname=slides_sr '\newif\ifsr\srtrue\newif\ifen\enfalse\input{slides.tex}'
