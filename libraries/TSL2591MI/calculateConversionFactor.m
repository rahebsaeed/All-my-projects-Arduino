function [conversionFactor, stdev] = calculateConversionFactor(irradiance, counts, a, t, spectrum, spectralResponsivity)
  %calculates sensor conversion facor. this value should be almost equal for both sensor channels. 
  %@param (array) irradiance in W/m^2
  %@param (array) counts sensor counts
  %@param a amplification setting (1, 25, 428, 9876)
  %@param t integration time multiplier (1, 2, ..., 6)
  %@param (matrix, lambda | value) spectrum
  %@param (matrix, lambda | value) spectral responsivity
  %@return [conversionFactor, stdev]
  %@return conversion factor in (W / m^2) / counts. 
  %@return standard deviation
  
  %match sensor and light data to have the same lambda step size
  lambdaStep = 10;
  lambdaMin = min(min(spectrum(:, 1)), min(spectralResponsivity(:, 1)));
  lambdaMax = max(max(spectrum(:, 1)), max(spectralResponsivity(:, 1)));
  newLambda = lambdaMin:lambdaStep:lambdaMax;
  
  %interpolate spectrum and spectral responsivity
  newSpectrum =  interp1(spectrum(:, 1), spectrum(:, 2), newLambda);
  newSpectralResponsivity =  interp1(spectralResponsivity(:, 1), spectralResponsivity(:, 2), newLambda);
  
  %avoid NANs 
  newSpectrum(isnan(newSpectrum)) = 0;
  newSpectralResponsivity(isnan(newSpectralResponsivity)) = 0;  
  
  newSpectrum = newSpectrum / trapz(newLambda, newSpectrum);
  
  %calculate conversion factors and standard deviation
  factor = counts ./ (trapz(newLambda, newSpectrum .* newSpectralResponsivity) .* irradiance * a * t);
  conversionFactor = mean(factor);
  stdev = std(factor);
end

%function conversionFactor = calculateConversionFactor(irradiance, countsCh0, countsCh1, a, t)
%  %calcualtes sensor conversion factor 
%  %@param (array) irradiance irradiance in W/m^2. 
%  %@param (array) countsCh0 channel 0 counts
%  %@param (array) countsCh1 channel 1 counts
%  %@param a amplification setting (1, 25, 428, 9876)
%  %@param t integration time multiplier (1, 2, ..., 6)
%  %@return (array) calculated conversion factors 
%
%  %match sensor and light data to have the same lambda step size
%  lmabdaStep = 10;
%  lambdaMin = 300;
%  lambdaMax = 3140;
%  newLambda = lambdaMin:lmabdaStep:lambdaMax;
%
%  %read sensor characteristics
%  sensor = csvread('TSL2591.csv');
%  sensorLambda = sensor(2:end, 1);
%  sensorCH0 = sensor(2:end, 2);
%  sensorCH1 = sensor(2:end, 3);
%
%  %interpolate sensor vectors
%  newSensorCH0 = interp1(sensorLambda, sensorCH0, newLambda);
%  newSensorCH1 = interp1(sensorLambda, sensorCH1, newLambda);
%  newSensorCH0(isnan(newSensorCH0)) = 0;
%  newSensorCH1(isnan(newSensorCH1)) = 0;
%
%  %read light characteristics
%  light = csvread('ISO 9845-1 1992 table.csv');
%  lightLambda = light(2:end, 1);
%  lightSpectralIrradiance = light(2:end, 2);
%  lightIrradiance = trapz(lightLambda, lightSpectralIrradiance);
%  lightSpectralIrradiance = lightSpectralIrradiance / lightIrradiance;
%
%  %interpolate light vector
%  newLight = interp1(lightLambda, lightSpectralIrradiance, newLambda);
%  newLight(isnan(newLight)) = 0;
%
%  %calculate irradiance conversion factor  
%  factorCh0 = (countsCh0 * a * t) ./ (trapz(newLambda, newLight .* newSensorCH0) * irradiance);
%  factorCh1 = (countsCh1 * a * t) ./ (trapz(newLambda, newLight .* newSensorCH1) * irradiance);
%  
%  conversionFactor = [factorCh0' factorCh1'];
%end