Action()
{


	// UUID example 9d77d210-0452-11ec-9a03-0242ac130003
	
	// Create UUID4 (Length of 4, values between 0000-ffff) and UUID8 (length of 8, values between 00000000-ffffffff ) randmom number parameters
	// Update value on each occurence
	
	lr_save_string(lr_eval_string("{UUID8}-{UUID4}-{UUID4}-{UUID4}-{UUID4}{UUID8}"), "UUID_P");
	lr_save_string(lr_eval_string("{UUID8}-{UUID4}-{UUID4}-{UUID4}-{UUID4}{UUID8}"), "UUID_P2");
	
	return 0;
}
