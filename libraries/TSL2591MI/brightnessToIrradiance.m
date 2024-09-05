function irradianceValues = brightnessToIrradiance(brightness, spectrum, luminousEfficiency)
  %calculates irradiance from a measured brightness value. 
  %@param (Array) brightness in lux. 
  %@param (matrix, lambda | value) spectrum
  %@param (matrix, lambda | value) spectral responsivity
  %@return irradiance in W/m^2. 

  Km = 683;  
  
  %match sensor and light data to have the same lambda step size
  lambdaStep = 10;
  lambdaMin = min(min(spectrum(:, 1)), min(luminousEfficiency(:, 1)));
  lambdaMax = max(max(spectrum(:, 1)), max(luminousEfficiency(:, 1)));
  newLambda = lambdaMin:lambdaStep:lambdaMax;
  
  %interpolate spectrum and spectral responsivity
  newSpectrum = interp1(spectrum(:, 1), spectrum(:, 2), newLambda);
  newLuminousEfficiency = interp1(luminousEfficiency(:, 1), luminousEfficiency(:, 2), newLambda);
    
  %avoid NANs 
  newSpectrum(isnan(newSpectrum)) = 0;
  newLuminousEfficiency(isnan(newLuminousEfficiency)) = 0;
  
  newSpectrum = newSpectrum / trapz(newLambda, newSpectrum);
  
  irradianceValues = brightness / (Km * trapz(newLambda, newSpectrum .* newLuminousEfficiency));
  irradiance = mean(irradianceValues);
  stdev = std(irradianceValues);
end

%function irradiance = brightnessToIrradiance(brightness)
%  %calculates irradiance from a measured brightness value. 
%  %@param brightness in lux. 
%  %@return irradiance in W/m^2. 
%  
%  %read light characteristics
%  light = csvread('ISO 9845-1 1992 table.csv');
%  lightLambda = light(2:end, 1);
%  lightSpectralIrradiance = light(2:end, 2);
%  lightIrradiance = trapz(lightLambda, lightSpectralIrradiance);
%  lightSpectralIrradiance = lightSpectralIrradiance / lightIrradiance;
%
%  Km = 683;
%
%  %read luminosity functions
%  cie = csvread('www.cvrl.org/linCIE2008v2e_10.csv');
%  cieLambda = cie(2:end, 1);
%  cieFactor = cie(2:end, 2);
%  
%  %match sensor and light data to have the same lambda step size
%  lmabdaStep = 10;
%  lambdaMin = 300;
%  lambdaMax = 3140;
%  newLambda = lambdaMin:lmabdaStep:lambdaMax;
%  
%  %interpolate luminosity functions
%  newCie = interp1(cieLambda, cieFactor, newLambda);
%  newCie(isnan(newCie)) = 0;
%
%  %interpolate light values
%  newLight = interp1(lightLambda, lightSpectralIrradiance, newLambda);
%  newLight(isnan(newLight)) = 0;
%  
%  irradiance = brightness / (Km * trapz(newLambda, newCie .* newLight));
%end