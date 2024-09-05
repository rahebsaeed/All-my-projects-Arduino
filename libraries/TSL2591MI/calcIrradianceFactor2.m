function [conversionFactor, stdev] = calcIrradianceFactor2(irradiance, counts, a, t)
  %calculates conversion factor between sensor counts and irradiance. 
  %uses alternative method (direct method)
  %@param (array) irradiance irradiance in W/m^2. 
  %@param (array) counts
  %@param a amplification setting (1, 25, 428, 9876)
  %@param t integration time multiplier (1, 2, ..., 6)
  %@return [conversionFactor, stdev]
  %@return conversion factor in (W / m^2) / counts. 
  %@return standard deviation
  
  %alternate method
  factor = irradiance ./ (counts * a * t);
  
  conversionFactor = mean(factor);
  stdev = std(factor);
end

%function factors = calcIrradianceFactor2(irradiance, countsCh0, countsCh1, a, t)
%  %calculates conversion factor between sensor counts and irradiance. 
%  %uses alternative method (direct method)
%  %@param (array) irradiance irradiance in W/m^2. 
%  %@param (array) countsCh0 channel 0 counts
%  %@param (array) countsCh1 channel 1 counts
%  %@param a amplification setting (1, 25, 428, 9876)
%  %@param t integration time multiplier (1, 2, ..., 6)
%  %@return (array) calculated conversion factors 
%  
%  %alternate method
%  factorCh0 = irradiance ./ (countsCh0 * a * t);
%  factorCh1 = irradiance ./ (countsCh1 * a * t);
%  
%  factors = [factorCh0' factorCh1'];
%end