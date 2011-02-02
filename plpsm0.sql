CREATE OR REPLACE FUNCTION pg_catalog.psm0_call_handler()
RETURNS language_handler
LANGUAGE c
AS '$libdir/psm0', $function$psm0_call_handler$function$;

CREATE OR REPLACE FUNCTION pg_catalog.psm0_validator(oid)
RETURNS language_handler
LANGUAGE c
AS '$libdir/psm0', $function$psm0_validator$function$;

CREATE OR REPLACE LANGUAGE psm0 HANDLER psm0_call_handler VALIDATOR psm0_validator;
