.directive

.text

label: mnemo1

function:

mnemo2_nospaces
 mnemo3_1space
  mnemo3_2spaces
	mnemo4_tab

 labelonespace:

  labeltwospaces:

	tablabel:

.section test

  spacelabel:   spacemnemo

  spacelabel:   spacemnemo2 arg, val
  spacelabel:   spacemnemo2 	argtab, val

mnemoimm       #hahaha
mnemocomment   @comment

mnemocomment arg   @comment

.data

label: mnemocommentlbl   @comment

label: mnemocommentlbl arg   @comment

labnospc:plop

.section .text

#from martial
label_no_space:@comment
label_no_space:mnemo_no_space@comment
mnemo_no_space@comment

.string "test"

.string "test1", "test2"
.string "test1" , "test2"
.data
.string "test1" , "test2"
.string "test1", "test2" 

.string error

