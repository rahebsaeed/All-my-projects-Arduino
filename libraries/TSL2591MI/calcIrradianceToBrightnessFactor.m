function factor = calcIrradianceToBrightnessFactor(spectrum, luminousEfficiency)
  %
  %
  
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
  
  factor = Km * trapz(newSpectrum .* newLuminousEfficiency);
end