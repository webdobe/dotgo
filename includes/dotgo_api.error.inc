<?php

/**
 * @param null $code
 0 @return array Dotgo Response Errors.
 */
function _dotgo_error_code($code = NULL) {

  $codes = array(
    0 => t('Success.'),
    1 => t('Unknown error.'),
    2 => t('Permission denied.'),
    3 => t('API access disabled, please contact suppoertdotgo.com.'),
    100 => t('Unknown method.'),
    101 => t('Invalid parameters.'),
    102 => t('Unknown format.'),
    103 => t('Invalid URL format.'),
    104 => t('Invalid value for limit.'),
    105 => t('Invalid value for offset.'),
    1000 => t('Message error.'),
    1001 => t('No user registered to path.'),
    1002 => t('Couldn\'t send node.'),
    1003 => t('Message already sent.'),
    1004 => t('Message already cancelled.'),
    1101 => t('Invalid send node.'),
    1102 => t('Invalid time.'),
    1103 => t('Invalid designator.'),
    1104 => t('Invalid channel.'),
    1105 => t('Invalid pathTo.'),
    1106 => t('Invalid message identifier.'),
    2000 => t('Account error.'),
    2100 => t('No such subaccount.'),
    2101 => t('Subaccount identifier exists.'),
    2102 => t('Cannot create subaccount with that name.'),
    2103 => t('Cannot destroy subaccount with premium services. First call deleteService.'),
    2200 => t('No such URL.'),
    2201 => t('URL already added.'),
    2202 => t('Invalid urlKey.'),
    2203 => t('Cannot destroy url with premium services. First call deleteService.'),
    3000 => t('Service error.'),
    3001 => t('Unknown service.'),
    3002 => t('Unable to enable service, please contact support@dotgo.com.'),
    3003 => t('A service has already been added for the given URL. Use modifyService or deleteService to change the service.'),
    3004 => t('A service has not yet been added for the given URL. Use addService to add a service.'),
    3005 => t('Request to modifyService failed because the new settings match the current settings.'),
    3100 => t('Coupon unknown.'),
    3101 => t('Coupon cannot be used yet.'),
    3102 => t('Coupon expired.'),
    3103 => t('Coupon has reached use limit.'),
    3104 => t('Coupon not applicable for given service.'),
    3105 => t('Coupon may not be used for messaging plans under reseller accounts.'),
    3106 => t('Coupon minimum cost not met.'),
    3200 => t('Service update error.'),
    3201 => t('Service updates completed.'),
    4000 => t('Payment error.'),
    4001 => t('Payment database error.'),
    4002 => t('No billing information.'),
    4003 => t('Invalid billing information'),
    4004 => t('Billing information declined.'),
    4005 => t('Payment gateway error'),
    4006 => t('Billing complete.'),
    5000 => t('CMRL error.'),
    5001 => t('Unable to access CMRL file.'),
    5002 => t('Invalid CMRL format.'),
    6000 => t('Affiliate error.'),
    6001 => t('Affiliate unknown.'),
    6002 => t('Cannot replace affiliate.'),
    6003 => t('Affiliate program has not begun.'),
    6004 => t('Affiliate program has ended.'),
  );

  if (isset($code)) {
    return $codes[$code];
  }
  else {
    return $codes;
  }
}