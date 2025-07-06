SECTIONS
{
  .shell_vector : 
  {
	  __shell_vector_start = .;
	  *(.shell_*);
	  __shell_vector_end = .;
  } >FLASH
}
