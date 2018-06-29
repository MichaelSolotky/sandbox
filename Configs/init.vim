if &compatible
  set nocompatible
endif

set runtimepath+=/usr/share/vim/vimfiles  " Place for all plugins from AUR and official repos

call dein#begin(expand('~/.cache/nvim/plugins')) 
call dein#add('Shougo/dein.vim')

" Completion
call dein#add('Shougo/deoplete.nvim')
if !has('nvim')
  call dein#add('roxma/nvim-yarp')
  call dein#add('roxma/vim-hug-neovim-rpc')
endif

" Completion language support
"call dein#add('zchee/deoplete-jedi')

call dein#add('Shougo/unite.vim')
call dein#add('chriskempson/base16-vim')
call dein#add('SirVer/ultisnips')
call dein#add('jiangmiao/auto-pairs')
call dein#add('Matt-Deacalion/vim-systemd-syntax')
call dein#add('janko-m/vim-test')
call dein#add('lervag/vimtex')
call dein#add('Shougo/neosnippet')
call dein#add('Shougo/neosnippet-snippets')

call dein#end()
call dein#save_state()

if dein#check_install()
  call dein#install()
endif

filetype plugin on
set background=light

let base16colorspace=256

" Set up indentation
set tabstop=2 softtabstop=2 shiftwidth=2 autoindent expandtab virtualedit=all wrap

autocmd FileType make set shiftwidth=4 noexpandtab


" Keybindings for vim-test
nmap <silent> <leader>t :TestNearest<CR>
nmap <silent> <leader>T :TestFile<CR>

" Completion setup
let g:deoplete#enable_at_startup = 1
let g:deoplete#sources#jedi#show_docstring = 1
let g:deoplete#sources#jedi#server_timeout = 20
set completeopt+=noinsert


" LaTeX completion
if !exists('g:deoplete#omni#input_patterns')
		let g:deoplete#omni#input_patterns = {}
endif
let g:deoplete#omni#input_patterns.tex = g:vimtex#re#deoplete
let g:vimtex_enabled = 1
let g:vimtex_complete_enabled = 1
let g:vimtex_complete_close_braces = 1
let g:vimtex_complete_recursive_bib = 1

imap <C-k>     <Plug>(neosnippet_expand_or_jump)
smap <C-k>     <Plug>(neosnippet_expand_or_jump)
xmap <C-k>     <Plug>(neosnippet_expand_target) 

" For conceal markers.
"if has('conceal')
"  set conceallevel=2 concealcursor=niv
"endif 

if &filetype == 'tex'
  :set spell spelllang=en,ru
  :set spell
  nmap <F10> :w<CR>:make<CR>
endif

"  :set spell spelllang=en,ru
"  :set spell
"  nmap <F10> :w<CR>:make<CR>

