<!DOCTYPE style-sheet PUBLIC
          "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY docbook.dsl PUBLIC
         "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN"
         CDATA dsssl>
]>
<style-sheet>

<style-specification id="html" use="docbook">
<style-specification-body> 

;; borrowed from ldp.dsl at http://www.tldp.org/authors/tools/ldp.dsl

;; ------------------------------------------------------------------------
;; ldp.dsl - LDP Customized DSSSL Stylesheet
;; v1.12, 2003-03-19
;; Copyright (C) 2000-2003
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
;; ------------------------------------------------------------------------

<!--
;; customize the html stylesheet; parts borrowed from 
;; Cygnus at http://sourceware.cygnus.com/ (cygnus-both.dsl)
-->

(declare-characteristic preserve-sdata?
  ;; this is necessary because right now jadetex does not understand
  ;; symbolic entities, whereas things work well with numeric entities.
  "UNREGISTERED::James Clark//Characteristic::preserve-sdata?"
  #f)

(declare-flow-object-class element
  ;; for redhat
  "UNREGISTERED::James Clark//Flow Object Class::element")

(define %generate-legalnotice-link%
  ;; put the legal notice in a separate file
  #t)

;(define %admon-graphics-path%
;  ;; use graphics in admonitions, set their
;  "../images/")


;(define %admon-graphics% #t)

(define %funcsynopsis-decoration%
  ;; make funcsynopsis look pretty
  #t)

(define %html-ext%
  ;; when producing HTML files, use this extension
  ".html")

(define %generate-book-toc%
  ;; Should a Table of Contents be produced for books?
  #t)

(define %generate-article-toc% 
  ;; Should a Table of Contents be produced for articles?
  #t)

(define %generate-part-toc%
  ;; Should a Table of Contents be produced for parts?
  #t)

(define %generate-book-titlepage%
  ;; produce a title page for books
  #t)

(define %generate-article-titlepage%
  ;; produce a title page for articles
  #t)

(define (chunk-skip-first-element-list)
  ;; forces the Table of Contents on separate page
  '())

(define (list-element-list)
  ;; fixes bug in Table of Contents generation
  '())

(define %root-filename%
  ;; The filename of the root HTML document (e.g, "index").
  "index")

(define %shade-verbatim%
  ;; verbatim sections will be shaded if t(rue)
  #t)

(define %use-id-as-filename%
  ;; Use ID attributes as name for component HTML files?
  #t)

(define %graphic-extensions%
  ;; graphic extensions allowed
  '("gif" "png" "jpg" "jpeg" "tif" "tiff" "eps" "epsf" ))

(define %graphic-default-extension% 
  "gif")

(define %section-autolabel%
  ;; For enumerated sections (1.1, 1.1.1, 1.2, etc.)
  #t)

(define (toc-depth nd)
  ;; more depth (2 levels) to toc; instead of flat hierarchy
  2)

(element emphasis
  ;; make role=strong equate to bold for emphasis tag
  (if (equal? (attribute-string "role") "strong")
     (make element gi: "STRONG" (process-children))
     (make element gi: "EM" (process-children))))

(define (book-titlepage-recto-elements)
  ;; elements on a book's titlepage
  (list (normalize "title")
        (normalize "subtitle")
        (normalize "graphic")
        (normalize "mediaobject")
        (normalize "corpauthor")
        (normalize "authorgroup")
        (normalize "author")
        (normalize "othercredit")
        (normalize "contrib")
        (normalize "edition")
        (normalize "releaseinfo")
        (normalize "publisher")
        (normalize "editor")
        (normalize "copyright")
        (normalize "pubdate")
        (normalize "revhistory")
        (normalize "abstract")
        (normalize "legalnotice")))

(define (article-titlepage-recto-elements)
  ;; elements on an article's titlepage
  (list (normalize "title")
        (normalize "subtitle")
        (normalize "authorgroup")
        (normalize "author")
        (normalize "othercredit")
        (normalize "releaseinfo")
        (normalize "copyright")
        (normalize "pubdate")
        (normalize "revhistory")
        (normalize "abstract")
        (normalize "legalnotice")))

(define (process-contrib #!optional (sosofo (process-children)))
  ;; print out with othercredit information; for translators, etc.
  (make sequence
    (make element gi: "SPAN"
          attributes: (list (list "CLASS" (gi)))
          (process-children))))

;(define (process-othercredit #!optional (sosofo (process-children)))
;  ;; print out othercredit information; for translators, etc.
;  (let ((author-name  (author-string))
;        (author-contrib (select-elements (children (current-node))
;                                          (normalize "contrib"))))
;    (make element gi: "P"
;         attributes: (list (list "CLASS" (gi)))
;         (make element gi: "B"
;              (literal author-name)
;              (literal " - "))
;         (process-node-list author-contrib))))

(mode article-titlepage-recto-mode
  (element contrib (process-contrib))
  (element othercredit (process-othercredit))
)

(mode book-titlepage-recto-mode
  (element contrib (process-contrib))
  (element othercredit (process-othercredit))
)

(define (article-title nd)
  (let* ((artchild  (children nd))
         (artheader (select-elements artchild (normalize "artheader")))
         (artinfo   (select-elements artchild (normalize "articleinfo")))
         (ahdr (if (node-list-empty? artheader)
                   artinfo
                   artheader))
         (ahtitles  (select-elements (children ahdr)
                                     (normalize "title")))
         (artitles  (select-elements artchild (normalize "title")))
         (titles    (if (node-list-empty? artitles)
                        ahtitles
                        artitles)))
    (if (node-list-empty? titles)
        ""
        (node-list-first titles))))


(mode subtitle-mode
  ;; do not print subtitle on subsequent pages
  (element subtitle (empty-sosofo)))


;; Redefinition of $verbatim-display$
;; Origin: dbverb.dsl
;; Different foreground and background colors for verbatim elements
;; Author: Philippe Martin (feloy@free.fr) 2001-04-07

(define ($verbatim-display$ indent line-numbers?)
  (let ((verbatim-element (gi))
        (content (make element gi: "PRE"
                       attributes: (list
                                    (list "CLASS" (gi)))
                       (if (or indent line-numbers?)
                           ($verbatim-line-by-line$ indent line-numbers?)
                           (process-children)))))
    (if %shade-verbatim%
        (make element gi: "TABLE"
              attributes: (shade-verbatim-attr-element verbatim-element)
              (make element gi: "TR"
                    (make element gi: "TD"
                          (make element gi: "FONT" 
                                attributes: (list
                                             (list "COLOR" (car (shade-verbatim-element-colors
                                                                 verbatim-element))))
                                content))))
        content)))

;;
;; Customize this function
;; to change the foreground and background colors
;; of the different verbatim elements
;; Return (list "foreground color" "background color")
;;
(define (shade-verbatim-element-colors element)
  (case element
    (("SYNOPSIS") (list "#000000" "#6495ED"))
    ;; ...
    ;; Add your verbatim elements here
    ;; ...
    (else (list "#000000" "#E0E0E0"))))

(define (shade-verbatim-attr-element element)
  (list
   (list "BORDER" 
        (cond
                ((equal? element (normalize "SCREEN")) "1")
                (else "0")))
   (list "BGCOLOR" (car (cdr (shade-verbatim-element-colors element))))
   (list "WIDTH" ($table-width$))))

;; End of $verbatim-display$ redefinition


;; jf-custom.dsl original

(define %admon-graphics-path%
  ;; use graphics in admonitions, set their
  "../images/")

(define %admon-graphics%
  #t)

(define %callout-graphics-path%
  ;; Sets the path, probably relative to the directory where the HTML
  ;; files are created, to the callout graphics.
  "./")

(define (process-othercredit #!optional (sosofo (process-children)))
  ;; print out othercredit information; for translators, etc.
  ;; note: added author-affil by JF.
  (let ((author-name  (author-string))
        (author-contrib (select-elements (children (current-node))
					 (normalize "contrib")))
	(author-affil (select-elements (children (current-node)) 
                                       (normalize "affiliation"))))

    (make element gi: "P"
         attributes: (list (list "CLASS" (gi)))
         (make element gi: "B"
              (literal author-name)
              (literal " - "))
         (process-node-list author-contrib)
	 (process-node-list author-affil))))

(define %show-ulinks%
  ;; REFENTRY show-ulinks
  ;; PURP Display URLs after ULinks?
  ;; DESC
  ;; If true, the URL of each ULink will appear in parenthesis after
  ;; the text of the link.  If the text of the link and the URL are
  ;; identical, the parenthetical URL is suppressed.
  ;; /DESC
  ;; AUTHOR N/A
  ;; /REFENTRY
  #f)

(element ulink 
  (make element gi: "A"
        attributes: (list
                     (list "HREF" (attribute-string (normalize "url")))
                     (list "TARGET" "_top"))
        (if (node-list-empty? (children (current-node)))
            (literal (attribute-string (normalize "url")))
            (process-children))
	(if (and %show-ulinks%
		 (not (node-list-empty? (children (current-node))))
		 (not (equal? (attribute-string (normalize "url"))
			      (data-of (current-node)))))
	    (make sequence
	      (literal " <")
	      (literal (attribute-string (normalize "url")))
	      (literal ">"))
	    (empty-sosofo))))

;; The following is obtained from:
;;
;; &#62; Id: dbbibl.dsl,v 1.23 2000/07/19 18:40:06 nwalsh Exp
;; &#62;
;; &#62; This file is part of the Modular DocBook Stylesheet distribution.
;;
;; and modified for Japanese name order convention.
;; (modified by Hiroki Sato &#60;hrs@jp.FreeBSD.org&#62;

(define (author-string #!optional (author (current-node)))
  ;; Return a formatted string representation of the contents of:
  ;; AUTHOR:
  ;;   Handles Honorific, FirstName, SurName, and Lineage.
  ;;     If %author-othername-in-middle% is #t, also OtherName
  ;;   Handles *only* the first of each.
  ;;   Format is "Honorific. FirstName [OtherName] SurName, Lineage"
  ;; CORPAUTHOR:
  ;;   returns (data corpauthor)
  (let* (
;	 (lang (attribute-string "lang" (select-elements 
;					 author (normalize "author"))))
	 (lang (if (string? author) ($lang$) ($lang$ author)))
         (h_nl (select-elements 
		(descendants author) (normalize "honorific")))
	 (f_nl (if (equal? lang "ja")
                   (select-elements 
		    (descendants author) (normalize "surname"))
                   (select-elements 
		    (descendants author) (normalize "firstname"))))
         (o_nl (select-elements 
		(descendants author) (normalize "othername")))
	 (s_nl (if (equal? lang "ja")
                   (select-elements 
		    (descendants author) (normalize "firstname"))
                   (select-elements 
		    (descendants author) (normalize "surname"))))
         (l_nl (select-elements 
		(descendants author) (normalize "lineage")))
	 (has_h (not (node-list-empty? h_nl)))
	 (has_f (not (node-list-empty? f_nl)))
	 (has_o (and %author-othername-in-middle%
		     (not (node-list-empty? o_nl))))
	 (has_s (not (node-list-empty? s_nl)))
	 (has_l (not (node-list-empty? l_nl))))
    (if (or (equal? (gi author) (normalize "author"))
	    (equal? (gi author) (normalize "editor"))
	    (equal? (gi author) (normalize "othercredit")))
	(string-append
	 (if has_h (string-append (data-of (node-list-first h_nl))
				  %honorific-punctuation%) "")
	 (if has_f (string-append
		    (if has_h " " "")
		    (data-of (node-list-first f_nl))) "")
	 (if has_o (string-append
		    (if (or has_h has_f) " " "")
		    (data-of (node-list-first o_nl))) "")
	 (if has_s (string-append
		    (if (or has_h has_f has_o) " " "")
		    (data-of (node-list-first s_nl))) "")
	 (if has_l (string-append
		    ", " (data-of (node-list-first l_nl))) ""))
	(data-of author))))

(element email
  ;; Do not link to mail address.
  ($mono-seq$
   (make sequence 
     (literal "&#60;")
     (process-children)
     (literal "&#62;"))))

(define %gentext-nav-tblwidth% "100%")

(define %honorific-punctuation% " / ")
(define biblio-number  #t)
(define %table-rules% #t)


(define %body-attr% 
  ;; What attributes should be hung off of BODY?
  (list
   (list "BGCOLOR" "#FFFFFF")
   (list "TEXT" "#000000")))

</style-specification-body>
</style-specification>

<external-specification id="docbook" document="docbook.dsl">

</style-sheet>
