#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

int main(int argc, char **argv)
{
	json_object *tmp=json_object_new_int(123);
	assert (json_object_get_int(tmp)==123);
	json_object_set_int(tmp,321);
	assert (json_object_get_int(tmp)==321); 
	printf("INT PASSED\n");
	json_object_set_int64(tmp,(int64_t)321321321);
	assert (json_object_get_int64(tmp)==321321321); 
	json_object_put(tmp);
	printf("INT64 PASSED\n");
	tmp=json_object_new_boolean(TRUE);
	assert (json_object_get_boolean(tmp)==TRUE); 
	json_object_set_boolean(tmp,FALSE);
	assert (json_object_get_boolean(tmp)==FALSE); 
	json_object_set_boolean(tmp,TRUE);
	assert (json_object_get_boolean(tmp)==TRUE); 
	json_object_put(tmp);
	printf("BOOL PASSED\n");
	
	
	printf("PASSED\n");
	return 0;
}
