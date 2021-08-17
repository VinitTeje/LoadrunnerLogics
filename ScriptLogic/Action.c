Action()
{	

	
	FindFirstLastDay();
	
	lr_save_datetime ("%d", DATE_NOW, "pDate4");
	lr_save_datetime ("%m/01/%Y", DATE_NOW-(ONE_DAY*(atoi(lr_eval_string ("{pDate4}"))+1)), "pDate5");
	lr_output_message ("First Day of Previous Month is %s", lr_eval_string ("{pDate5}"));

	lr_save_datetime ("%m/01/%Y", DATE_NOW+(ONE_DAY*31), "pDate6");
	lr_output_message ("First Day of next Month is %s", lr_eval_string ("{pDate6}"));


//	char AR[] = {1,2};

	getWeeklyDates(3);
		
	
//	lr_save_string("dasda,sdcbasdhbahu", "PARAMETER");
	lr_save_string("dasda,sdcbasdhbahu,cashbashcb,sdbcasdjbcasd,easdasdaswd,sdfwsdfwef,fwdfwefwerf,fefsdfs,df,ggrtgedfg,werwerwefct,fgvedf", "PARAMETER");

	lr_save_param_regexp(lr_eval_string("{PARAMETER}"), strlen(lr_eval_string("{PARAMETER}")),
               "RegExp=(.*?),",
               "Ordinal=All",
               "ResultParam=OUTPUT_PARAM",
               LAST );

	SaveArrayToParamter(lr_eval_string("{PARAMETER}"), ",", "OUTPUT_ID");

	SaveArrayToCustomString("[option value=\"", "\" option value=\"", "]", "OUTPUT_ID", "Custom_Body_Param_Request1");

	lr_save_string("State ID", "isSuccess");
	lr_replace("isSuccess", " ", "%20");
	
	Start_MasterTransaction();

	lr_save_timestamp("Timestamp", "DIGITS=13", LAST );
	lr_save_string("1", "isSuccess");

	Start_Transaction("01_HOMEPAGE");
	ErrorCheck(LR_EXIT_MAIN_ITERATION_AND_CONTINUE);	


	lr_end_transaction(lr_eval_string("{TRANSNAME}"), LR_AUTO);	
	
	lr_save_string("1", "isSuccess");
	Start_Transaction("02_LOGIN");
	ErrorCheck(LR_EXIT_MAIN_ITERATION_AND_CONTINUE);
	lr_end_transaction(lr_eval_string("{TRANSNAME}"), LR_AUTO);	

	
	
//	buildRandomZonesList(5, "ZonesList");		

	End_MasterTrasaction();

	
	
	return 0;
}
