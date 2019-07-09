
void active_delay(int msecs){
    auto start = std::chrono::high_resolution_clock::now();
    auto end = false;
    while(!end){
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        if(msec>msecs)
            end = true;
    }
    return;
}

auto translate_char(char c){
    active_delay(1);
    if(islower(c))
        return toupper(c);
    else
        return tolower(c);
}