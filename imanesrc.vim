" File Name: imanesrc.vim
" Maintainer: Rodrigo Tobar <rtobar at csrg.inf.utfsm dot org>
" Original Date: 2011-06-06
" Description: imanes.rc syntax

syntax keyword rcStdId square1
syntax keyword rcStdId square2
syntax keyword rcStdId triangle
syntax keyword rcStdId dmc
syntax keyword rcStdId noise

syntax match rcComment /#.*$/ display

syntax sync fromstart

let b:current_syntax = 'imanesrc'
