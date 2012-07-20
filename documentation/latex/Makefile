all: clean bQella-documentation.pdf

pdf: bQella-documentation.pdf

bQella-documentation.pdf: output/bQella-documentation.tex
	pdflatex output/bQella-documentation.tex
	makeindex output/bQella-documentation.idx
	pdflatex output/bQella-documentation.tex
	latex_count=5 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' output/bQella-documentation.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      pdflatex output/bQella-documentation.tex ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done


clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out output/bQella-documentation.pdf
