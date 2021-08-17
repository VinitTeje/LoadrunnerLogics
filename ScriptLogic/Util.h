

//-------------------Start a transaction--------------------------------------------//
void Start_Transaction(char *StepName)
{
	lr_think_time(1);	
	lr_param_sprintf("TRANSNAME", "%s_%s_%s", lr_eval_string("{APP}"), lr_eval_string("{SCRIPTNAME}"), StepName);	
	lr_start_transaction(lr_eval_string("{TRANSNAME}"));
}



//--------------------Custom Error Check with Error handling------------------------------------------------------//
void ErrorCheck(int ErrorCode)
{
	if (atoi(lr_eval_string("{isSuccess}")) == 0)
	{			    
		lr_fail_trans_with_error("ERROR: Failed Transaction \"%s\", %s, Iteration: %s", lr_eval_string("{TRANSNAME}"), lr_eval_string("{CURRENTTIME}"), lr_eval_string("{ITERATION}"));
		lr_exit(ErrorCode, LR_FAIL);
	}
}



//--------------------------Start Master transaction (Iteration)-------------------------------------------------//
void Start_MasterTransaction()
{
	lr_param_sprintf("MASTER_TRANSNAME", "%s", lr_eval_string("{SCRIPTNAME}"));	
	lr_start_transaction(lr_eval_string("{MASTER_TRANSNAME}"));
}




//--------------------------End Master Transaction (iteration)-----------------------------------//
void End_MasterTrasaction()
{
	lr_end_transaction(lr_eval_string("{MASTER_TRANSNAME}"), LR_AUTO);
	lr_vuser_status_message("Completed: %s%s for User: %s", lr_eval_string("{MASTER_TRANSNAME}"), lr_eval_string("{ITERATION}"), lr_eval_string("{VUSERID}"));

}
	


//---------------------------- Function to change Log Level on the fly-------------------------------------------          
void debug(int Option)
{
	if(Option == 0){ // ------------Disable Log--------------
		lr_set_debug_message(LR_MSG_CLASS_DISABLE_LOG, LR_SWITCH_ON);
		lr_log_message("Current Log Level is %d", lr_get_debug_message());
		}

	else if(Option == 1){ // --------------Standard Log-----------
		lr_set_debug_message(LR_MSG_CLASS_BRIEF_LOG, LR_SWITCH_ON);
		lr_log_message("Current Log Level is %d", lr_get_debug_message());
		}

	else if(Option == 2){ // --------------Extended Log Parameter Sub----------------
		lr_set_debug_message(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS, LR_SWITCH_ON);
		lr_log_message("Current Log Level is %d", lr_get_debug_message());
		}
	
	else if(Option == 3){ // ----------Extended Log: Parameters, Data Returned.-----------------
		lr_set_debug_message(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA, LR_SWITCH_ON );
		lr_log_message("Current Log Level is %d", lr_get_debug_message());
	}

	else if(Option == 4){ // -----------Extended Log: Parameters, Advanced Trace-------------------
		lr_set_debug_message(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA| LR_MSG_CLASS_FULL_TRACE, LR_SWITCH_ON );
		lr_log_message("Current Log Level is %d", lr_get_debug_message());
	}
	
	else{
		lr_error_message("ERROR: Invalid Debug Option Selected. Select 0 - 4");
		lr_abort();
	}
}




//---------------------------Replace string with another string inside parameter-----------------------------------------//
//Copied From : https://ptfrontline.wordpress.com/2009/03/13/search-replace-function-for-lr/
// Description:
//    Search for and replace text within a string.
//
// Parameters:
//    src (in) - pointer to source string
//    from (in) - pointer to search text
//    to (in) - pointer to replacement text
//
// Returns:
//    Returns a pointer to dynamically-allocated memory containing string
//    with occurences of the text pointed to by 'from' replaced by with
//    the text pointed to by 'to'.
//
// Notes:
//    Do not use this directly in scripts unless you are a more advanced
//    user who knows C and string handling. See below for the function you
//    should use!
//
// ----------------------------------------------------------------------------
char *strReplace(const char *src, const char *from, const char *to)
{
  char *value;
  char *dst;
  char *match;
  int size;
  int fromlen;
  int tolen;
 
  // Find out the lengths of the source string, text to replace, and
  // the replacement text.
  size = strlen(src) + 1;
  fromlen = strlen(from);
  tolen = strlen(to);
 
  // Allocate the first chunk with enough for the original string.
  value = (char *)malloc(size);
 
  // We need to return 'value', so let's make a copy to mess around with.
  dst = value;
 
  // Before we begin, let's see if malloc was successful.
  if ( value != NULL )
  {
    // Loop until no matches are found.
    for ( ;; )
    {
      // Try to find the search text.
      match = (char *) strstr(src, from);
      if ( match != NULL )
      {
        // Found search text at location 'match'.
        // Find out how many characters to copy up to the 'match'.
        size_t count = match - src;
 
        // We are going to realloc, and for that we will need a
        // temporary pointer for safe usage.
        char *temp;
 
        // Calculate the total size the string will be after the
        // replacement is performed.
        size += tolen - fromlen;
 
        // Attempt to realloc memory for the new size.
        //
        // temp = realloc(value, size);
        temp = (char *)realloc(value, size);
 
        if ( temp == NULL )
        {
          // Attempt to realloc failed. Free the previously malloc'd
          // memory and return with our tail between our legs.
          free(value);
          return NULL;
        }
 
        // The call to realloc was successful. But we'll want to
        // return 'value' eventually, so let's point it to the memory
        // that we are now working with. And let's not forget to point
        // to the right location in the destination as well.
        dst = temp + (dst - value);
        value = temp;
 
        // Copy from the source to the point where we matched. Then
        // move the source pointer ahead by the amount we copied. And
        // move the destination pointer ahead by the same amount.
        memmove(dst, src, count);
        src += count;
        dst += count;
 
        // Now copy in the replacement text 'to' at the position of
        // the match. Adjust the source pointer by the text we replaced.
        // Adjust the destination pointer by the amount of replacement
        // text.
        memmove(dst, to, tolen);
        src += fromlen;
        dst += tolen;
      }
      else // No match found.
      {
        // Copy any remaining part of the string. This includes the null
        // termination character.
        strcpy(dst, src);
        break;
      }
    } // For Loop()
  }
  return value;
}
 
// ----------------------------------------------------------------------------
//
// Description:
//    Find and replace text within a LoadRunner string.
//
// Parameters:
//    lrparam (in)    - pointer to LoadRunner Parameter Name
//    findstr (in)    - pointer to text top search for
//    replacestr (in) - pointer to text to use as replacement
//
// Returns:
//    Returns an integer. 0=Error, 1=Success.
//
// Example:
//    lr_save_string( "This is a small test of the search and replace function", "LRParam");
//    lr_replace( "LRParam", "a", "-x-" );
//    lr_output_message( "%s", lr_eval_string("{LRParam}") );
//
// ----------------------------------------------------------------------------
int lr_replace( const char *lrparam, char *findstr, char *replacestr )
{
  int res = 0;
  char *result_str;
  char lrp[1024];
 
  // Finalize the LR Param Name
  sprintf( lrp, "{%s}", lrparam);
 
  // Do the Search and Replace
  result_str = strReplace( lr_eval_string(lrp), findstr, replacestr );
 
  // Process results
  if (result_str != NULL )
  {
    lr_save_string( result_str, lrparam );
    free( result_str );
    res = 1;
  }
  return res;
} // EOF




SaveArrayToParamter(lr_eval_string("{PARAMETER}"), ",", "OUTPUT_ID");

// ------------ Save comma seperated value String tokens to parameters.-------------------------------
SaveArrayToParamter(char *capturedString, char *seperator, char *outVarName)
{
	char *token;
	int count=1;
	char *tempvarName;

	tempvarName = (char *)malloc((strlen(outVarName) + 10)* sizeof(char));
//	lr_output_message("tempvarName length: %d", strlen(tempvarName));

	token = (char *)strtok(capturedString, seperator); // Get the first token		
	if (!token){
		lr_error_message("ERROR: No tokens found in string!");
	    return(-1);
	    }
	
	while(token != NULL)
	{ 	// While valid tokens are returned
		sprintf(tempvarName, "%s_%d", outVarName, count++);
//		lr_save_string(token, lr_eval_string(tempvarName));
		lr_save_string(token, *&tempvarName);		
        token = (char *)strtok(NULL, seperator); // Get the next token 
	}

	sprintf(tempvarName, "%s_count", outVarName);
	lr_save_int(count-1, *&tempvarName);

	return 0;
}



SaveArrayToCustomString(char *startString, char *midString, char *endString, char *inputParam, char *outBody)
{
	char *outString;	// buffer to store the output string
	int outStringlen, loopCount=1;
	char *outBody1;    // buffer to store the name of output parameter.
	int orderCount = strlen(*&inputParam); // Array count of the parameter

	for (loopCount = 1; loopCount<=orderCount; loopCount++) // To Calculate buffer length for outString.
	{
		if(loopCount == 1){outStringlen = strlen(startString) + strlen(endString);}
		outStringlen = outStringlen + strlen(midString) + strlen(lr_eval_string(lr_paramarr_idx("OUTPUT_ID", loopCount)));		
	}
	
	outString = (char *)malloc(outStringlen * sizeof(char));
	strcpy(outString, "");

	strcat(outString, startString); // Append the Starting string to the variable.
//	lr_output_message("Outstring is %s", outString);
	
	for (loopCount = 1; loopCount < orderCount; loopCount++) // Conitnue append loop for all array values execpt last.
	{
		strcat(outString, lr_eval_string(lr_paramarr_idx("OUTPUT_ID", loopCount)));
		strcat(outString, midString);
	}
	
	// Append Last Value.
	strcat(outString, lr_eval_string(lr_paramarr_idx("OUTPUT_ID", orderCount)));
	// Append the ending string.
	strcat(outString, endString);
//	lr_output_message("Outstring is %s", outString);

	outBody1 = (char *)malloc(strlen(outString) * sizeof(char)); // buffer memory allocation to output parameter.	
	sprintf(outBody1, "\"%s\"", outBody);
	lr_save_string(outString, *&outBody1);
	free(outString);  // Empty string
	free(outBody1);
	
	return 0;
}






getWeeklyDates(int numofWeeks)
{
	int loopCount = 1; 
	getNextMonday("STARTDATE_1");
	
	for (loopCount = 1; loopCount<=numofWeeks ; loopCount++)
	{

		lr_param_sprintf("TempDATE", "ENDDATE_%d", loopCount);
		Add_Ndays(13 + 14 * (loopCount - 1), lr_eval_string("{TempDATE}"));

		while(loopCount < numofWeeks){
			lr_param_sprintf("TempDATE", "STARTDATE_%d", loopCount+1);
			Add_Ndays(14 + 14 * (loopCount - 1), lr_eval_string("{TempDATE}"));
			break;
		}
	}
	
	return 0;
}

getNextMonday(char *OutVar)
{
	lr_save_datetime("%A", DATE_NOW, "DAY");  // if not defined in parameter.

	if (strcmp(lr_eval_string("{DAY}"), "Monday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW + 86400*7, "TEMP_VAR");
			lr_save_int( 7, "EX_DAYS");
	}
	
	else if (strcmp(lr_eval_string("{DAY}"), "Tuesday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW + 86400*6, "TEMP_VAR");
			lr_save_int( 6, "EX_DAYS");
	}

	else if (strcmp(lr_eval_string("{DAY}"), "Wednesday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW + 86400*5, "TEMP_VAR");
			lr_save_int( 5, "EX_DAYS");
	}

	else if (strcmp(lr_eval_string("{DAY}"), "Thursday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW + 86400*4, "TEMP_VAR");
			lr_save_int( 4, "EX_DAYS");
	}

	else if	(strcmp(lr_eval_string("{DAY}"), "Friday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW +86400*3, "TEMP_VAR");
			lr_save_int( 3, "EX_DAYS");
	}

	else if	(strcmp(lr_eval_string("{DAY}"), "Saturday") == 0)
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW +86400*2, "TEMP_VAR");
			lr_save_int( 2, "EX_DAYS");
	}
	
	else 
	{
			lr_save_datetime( "%m/%d/%y" , DATE_NOW + 86400, "TEMP_VAR");
			lr_save_int( 1, "EX_DAYS");
	}
	
	lr_save_string(lr_eval_string("{TEMP_VAR}"), OutVar);
	
	return 0;
}



Add_Ndays(int NumDays, char *OutPARAM) // Add 
{
//	int daysToAdd = NumDays + atoi(lr_eval_string("{EX_DAYS}"));
//	lr_save_datetime( "%m/%d/%y" , 86400 * daysToAdd , OutPARAM);	
	lr_save_datetime( "%m/%d/%y" , 86400 * (NumDays + atoi(lr_eval_string("{EX_DAYS}"))), OutPARAM);
	return 0;
}


FindFirstLastDay()		
	{
		// to find last day of current date
	int i=0, j, count = 0;
	
	while(i!=1)
	{
		lr_save_datetime ("%d", DATE_NOW+(ONE_DAY*count), "pDate5");
		i = atoi(lr_eval_string ("{pDate5}"));	
		count++;
	}


	lr_save_datetime ("%d-%m-%Y", DATE_NOW + ONE_DAY*(count-2), "LastdayCurrentMonth");
	lr_save_datetime ("%d", DATE_NOW, "pDate4");  // Current date
	j = atoi(lr_eval_string ("{pDate4}"));
	lr_save_datetime ("%d-%m-%Y", DATE_NOW - ONE_DAY*(j - 1), "FirstDayCurrentMonth");
	lr_save_datetime ("%d-%m-%Y", DATE_NOW - (ONE_DAY*j), "LastdayLastMonth");

}






buildRandomZonesList(int zoneToSelect, char *outVarName){
	
    int i;
	char *zone, *selectedZones, *selectedZoneNames;


	// Declare sizes to hold the output variable values............
	zone = (char *)malloc(6 * zoneToSelect * sizeof(char));
	selectedZones = (char *)malloc(7 * zoneToSelect * sizeof(char));
	selectedZoneNames = (char *)malloc(50 * zoneToSelect * sizeof(char));

	// clear string........
	strcpy(zone, ""); 
	strcpy(selectedZones, ""); 
	strcpy(selectedZoneNames, "");

	// If # of zones to select is greater than PresalesRankerOperativeZone array length.....
	if(zoneToSelect >= lr_paramarr_len("PresalesRankerOperativeZone"))
	{
		// Build zone from available list only.
		for(i=1; i<=lr_paramarr_len("PresalesRankerOperativeZone"); i++){
			

			lr_save_string(lr_paramarr_idx("PresalesRankerOperativeZone", i), "PresalesRankerOperativeZone_P");

			lr_save_param_regexp(lr_eval_string("{PresalesRankerOperativeZone_P}"), strlen(lr_eval_string("{PresalesRankerOperativeZone_P}")),
                     "RegExp=value=\"(.+?)\"","Ordinal=1","ResultParam=PresalesRankerOperativeZoneID",LAST);
	
	    	lr_save_param_regexp(lr_eval_string("{PresalesRankerOperativeZone_P}"), strlen(lr_eval_string("{PresalesRankerOperativeZone_P}")),
                     "RegExp=> (.+?)\r\n","Ordinal=1","ResultParam=PresalesRankerOperativeZoneName",LAST);

			strcat(zone, "&SelectedZones[]=");
			strcat(zone, lr_eval_string("{PresalesRankerOperativeZoneID}"));

			strcat(selectedZones, lr_eval_string("\"{PresalesRankerOperativeZoneID}\""));
			strcat(selectedZoneNames, lr_eval_string("{PresalesRankerOperativeZoneName}"));
				if(i<lr_paramarr_len("PresalesRankerOperativeZone")){
			       	strcat(selectedZones, ",");
			       	strcat(selectedZoneNames, ", ");
			       }
		}
	}
	else
	{
		// Build string as per zone count value.....
		int RandNum = (int)(rand()%(lr_paramarr_len("PresalesRankerOperativeZone") - zoneToSelect))+1;
	    lr_output_message("The value of Random to select is %d", RandNum);
		
		for(i=1;i<=zoneToSelect; i++){
	    	lr_save_string(lr_paramarr_idx("PresalesRankerOperativeZone", RandNum), "PresalesRankerOperativeZone_P");

	    	lr_save_param_regexp(lr_eval_string("{PresalesRankerOperativeZone_P}"), strlen(lr_eval_string("{PresalesRankerOperativeZone_P}")),
                     "RegExp=value=\"(.+?)\"","Ordinal=1","ResultParam=PresalesRankerOperativeZoneID",LAST);
	
	    	lr_save_param_regexp(lr_eval_string("{PresalesRankerOperativeZone_P}"), strlen(lr_eval_string("{PresalesRankerOperativeZone_P}")),
                     "RegExp=> (.+?)\r\n","Ordinal=1","ResultParam=PresalesRankerOperativeZoneName",LAST);

	    	strcat(zone, "&SelectedZones[]=");
			strcat(zone, lr_eval_string("{PresalesRankerOperativeZoneID}"));			
			strcat(selectedZones, lr_eval_string("\"{PresalesRankerOperativeZoneID}\""));
			strcat(selectedZoneNames, lr_eval_string("{PresalesRankerOperativeZoneName}"));
			       if(i<zoneToSelect){
			       	strcat(selectedZones, ",");
			       	strcat(selectedZoneNames, ", ");
			       }
			
			RandNum++;
		}
	}
	
	lr_save_string(zone, outVarName);
	free(zone);
	lr_save_string(selectedZones, "selectedZones");
	free(selectedZones);
	lr_save_string(selectedZoneNames, "selectedZoneNames");
	free(selectedZoneNames);

	return 0;
}





