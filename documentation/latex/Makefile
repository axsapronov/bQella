all: clean bQella-documentation.pdf

pdf: bQella-documentation.pdf

bQella-documentation.pdf: bQella-documentation.tex
	pdflatex bQella-documentation.tex
	makeindex bQella-documentation.idx
	pdflatex bQella-documentation.tex
	latex_count=5 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' bQella-documentation.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      pdflatex bQella-documentation.tex ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done


clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out bQella-documentation.pdf
