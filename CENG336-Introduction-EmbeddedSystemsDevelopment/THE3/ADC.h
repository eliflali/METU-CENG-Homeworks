/* 
 * File:   ADC.h
 *
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

    void readADCChannel(unsigned char channel)
    {
        // 0b 0101 -> 5th chanel 
        ADCON0bits.CHS0 =  channel & 0x1; // Select channel..
        ADCON0bits.CHS1 = (channel >> 1) & 0x1;
        ADCON0bits.CHS2 = (channel >> 2) & 0x1;
        ADCON0bits.CHS3 = (channel >> 3) & 0x1;
        
        
        
        ADCON0bits.GODONE = 1; //Start conversion
        
        return; 
        
        
    }

    
    void initADC()
    {
        ADCON1bits.PCFG3 = 1; // RA0 = Analog, RA1=1 Analog, RA2 = Analog
        ADCON1bits.PCFG2 = 1;
        ADCON1bits.PCFG1 = 0;
        ADCON1bits.PCFG0 = 0;
        
        ADCON1bits.VCFG0 = 0; // Vref+=5.0, Vref=0
        ADCON1bits.VCFG1 = 0;
        
        TRISAbits.RA0 = 1;
        TRISAbits.RA1 = 1;
        TRISAbits.RA2 = 0;
        
        // For 40MHZ -> Tosc = 1/40 us
        // Tad options (2xTosc, 4xTosc, 8xTosc, 16xTosc, 32xTosc, 64xTosc)
        // min Tad 0.7 us - max Tad 25 us (Keep as short as possible)
        // The closest one to min Tad (32xTosc) hence Tad = 32xTosc = 0.8 us (ADCS2:ADCS0=010)
        // Acquisition time options (0xTad, 2xTad, 4xTad, 6xTad, 8xTad, 12xTad, 16xTad, 20xTad)
        // Min acquisition time = 2.4 us (the closest acquisition time we can set 4xTad = 3.2us) (ACQT2:ACQT0=010)
        
        //I should make Tad 8 - 100
        //
        
        ADCON2bits.ADCS2 = 1; // Tad (8xTOSC) -> 0.8us
        ADCON2bits.ADCS1 = 0;
        ADCON2bits.ADCS0 = 0;
        
        ADCON2bits.ACQT2 = 0; // Acquisition time (4xTad) = 3.2 us
        ADCON2bits.ACQT1 = 1;
        ADCON2bits.ACQT0 = 0;
        
        ADCON2bits.ADFM = 1; // Right justified...
                
        PIE1bits.ADIE = 1; 
        
        ADCON0bits.ADON = 1; // ADC module is enabled....
           
    }

    
#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

