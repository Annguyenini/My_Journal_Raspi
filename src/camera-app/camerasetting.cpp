
void CameraWorker::setBrightness (float value){
    BRIGHTNESS = value;
    this->applySettings();
    return;
};
void CameraWorker::setContrast (float value){
    CONTRAST = value;
    this->applySettings();
    return;
};
void CameraWorker::setSaturation (float value){
    SATURATION = value;
    this->applySettings();

    return;
};
void CameraWorker::setISO(float value){
    ISO = value;
    this->applySettings();

    return;
};
void CameraWorker::setExposure(float value){
    EXPOSURE = value;
    this->applySettings();

    return;
};
void CameraWorker::setRedGain(float rGain){
    
    R_GAIN = rGain;
    this->applySettings();
    return;
};    
void CameraWorker::setBlueGain(float bGain){
    
    B_GAIN = bGain;
    this->applySettings();
    return;
};   

void CameraWorker::applySettings(){
    _cameraControls->set(libcamera::controls::Brightness, BRIGHTNESS);
    _cameraControls->set (libcamera::controls::Contrast,CONTRAST);
    _cameraControls->set(libcamera::controls::Saturation, SATURATION);
    _cameraControls->set(libcamera::controls::AnalogueGain, ISO /100.0f);
    _cameraControls->set (libcamera::controls::ExposureTime,EXPOSURE);
    _cameraControls->set(libcamera::controls::ColourGains, ColourGains(R_GAIN, B_GAIN));
    _camera->setControl(_cameraControls.get());
}

void Camera::cameraSettingsLayout(){
    _settingBarSlider= new QHBoxLayout();
    _settingBarLabel = new QHBoxLayout();
    this->setUpBrightnessSlider();
    this->setUpContrastSlider();
    this->setUpSaturationSlider();
    this->setUpISO();
    this->setUpExposureSlider();
    this->setUpColorGain();
    _settingBarSlider->setAlignment(Qt::AlignCenter);
    _CameraLabel->addLayout(_settingBarSlider);   
       
}
void Camera::setUpBrightnessSlider(){
    _brightnessSlider = new QSlider(Qt::Vertical);
    _brightnessSlider ->setMinimum(-100);
    _brightnessSlider ->setMaximum(200);
    _brightnessSlider ->setValue(0);
    _brightnessSlider->setFixedHeight(150);
    _brightnessSlider->setTickPosition(QSlider::TicksBothSides);


    _brightnessLabel =new QLabel();
    _brightnessLabel ->setText(QString("Brightness: ")+QString::number(_brightnessSlider->value()));


    _settingBarLabel ->addWidget(_brightnessLabel);
    _settingBarSlider->addWidget(_brightnessSlider);
    connect(_brightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        _camera->setBrightness(value / 100.0f);
        _brightnessLabel ->setText(QString("Brightness: ")+QString::number(_brightnessSlider->value()));
    }); 
}
void Camera::setUpContrastSlider(){
    _contrastSlider = new QSlider(QT::Vertical);
    _contrastSlider->setMinimum(-100);
    _contrastSlider->setMaximum(200);
    _contrastSlider->setValue(0);
    _contrastSlider->setFixedHeight(150);
    _contrastSlider->setTickPosition(QSlider::TicksBothSides);

    _contrastLabel =new QLabel();
    _contrastLabel ->setText(QString("Contrast: ")+QString::number(_contrastSlider->value()));

    _settingBarLabel ->addWidget(_contrastLable);
    _settingBarSlider->addWidget(_contrastSlider);
    connect(_contrastSlider, &QSlider::valueChanged,this, [this](int value){
        _camera->setContrast(value/100.0f)
        _contrastLabel ->setText(QString("Contrast: ")+QString::number(_contrastSlider->value()));

    })
}
void Camera::setUpSaturationSlider(){
    _saturationSlider = new QSlider(Qt::Vertical);
    _saturationSlider->setMinimum(-100);
    _saturationSlider->setMaximum(200);
    _saturationSlider->setValue(0);
    _saturationSlider->setFixedHeight(150);
    _saturationSlider->setTickPosition(QSlider::TicksBothSides);
    _settingBarSlider->addWidget(_saturationSlider);
    connect(_saturationSlider,&QSlider::valueChanged,this,[this](int value){
        _camera ->setSaturation(value /100.0f);
    });
}
void Camera::setUpISO(){
    _isoSlider = new QSlider(Qt::Vertical);
    _isoSlider->setMinimum(100);
    _isoSlider->setMaximum(800);
    _isoSlider->setValue(100);
    _isoSlider->setFixedHeight(150);
    _isoSlider->setTickPosition(QSlider::TicksBothSides);

    _isoLabel =new QLabel();
    _isoLabel ->setText(QString("ISO: ")+QString::number(_isoSlider->value()));
    
    _settingBarLabel ->addWidget(_isoLable);
    _settingBarSlider->addWidget(_isoSlider);
    connect(_isoSlider,&QSlider::valueChanged,this,[this](int value)(){
        _camera ->setISO(value /100.0f);
        _isoLabel ->setText(QString("ISO: ")+QString::number(_isoSlider->value()));

    });
}
void Camera::setUpExposureSlider(){
    _exposureSlider = new QSlider(Qt::Vertical);
    _exposureSlider->setMinimum(-1000);
    _exposureSlider->setMaximum(1000);
    _exposureSlider->setValue(0);
    _exposureSlider->setFixedHeight(150);
    _exposureSlider->setTickPosition(QSlider::TicksBothSides);
    _exposureLabel =new QLabel();
    _exposureLabel ->setText(QString("Exposure: ")+QString::number(_exposureSlider->value()));
    
    _settingBarLabel ->addWidget(_exposureLable);
    _settingBarSlider->addWidget(_exposureSlider);
    connect(_exposureSlider,&QSlider::valueChanged,this,[this](int value){
        _camera ->setExposure(value /100.0f);
        _exposureLabel ->setText(QString("Exposure: ")+QString::number(_exposureSlider->value()));

    });
}
void Camera::setUpColorGain(){
    _bGainSlider = new QSlider(Qt::Vertical);
    _bGainSlider->setMinimum(0);
    _bGainSlider->setMaximum(100);
    _bGainSlider->setValue(50);
    _bGainSlider->setFixedHeight(150);
    _bGainSlider->setTickPosition(QSlider::TicksLeft);
    _rGainSlider = new QSlider(Qt::Vertical);
    _rGainSlider->setMinimum(0);
    _rGainSlider->setMaximum(100);
    _rGainSlider->setValue(50);
    _rGainSlider->setFixedHeight(150);
    _rGainSlider->setTickPosition(QSlider::TicksRight);
    _rGainLabel =new QLabel();
    _rGainLabel ->setText(QString("rGain: ")+QString::number(_rGainSlider->value()));
    _bGainLabel =new QLabel();
    _bGainLabel ->setText(QString("bGain: ")+QString::number(_bGainSlider->value()));
    _settingBarLabel ->addWidget(_rGainLable);
    _settingBarLabel ->addWidget(_bGainLable);
    _settingBarSlider->addWidget(_bGainSlider);
    _settingBarSlider->addWidget(_rGainSlider);
    connect(_bGainSlider,&QSlider::valueChanged,this,[this](int value){
        _camera ->setBlueGain(0.0f , value/ 100.0f);
        _bGainLabel ->setText(QString("bGain: ")+QString::number(_bGainSlider->value()));


    });
    connect(_rGainSlider,&QSlider::valueChanged,this,[this](int value){
        _camera ->setRedGain( value/ 100.0f, 0.0f);
        _rGainLabel ->setText(QString("rGain: ")+QString::number(_rGainSlider->value()));

        
    })
}
