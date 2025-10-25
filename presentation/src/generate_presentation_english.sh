#!/bin/bash

pdflatex -output-directory=.. -jobname=slides_en '\newif\ifen\entrue\newif\ifsr\srfalse\input{slides.tex}'
