
long TimeDifference(unsigned long prev, unsigned long next){
  long signed_diff = 0;
  const unsigned long half_max_unsigned_long = 2147483647u;
  if(next >= prev){
    const unsigned long diff = next - prev;
    if(diff <= half_max_unsigned_long){
      signed_diff = static_cast<long>(diff);
    }
    else{
      signed_diff = static_cast<long>((0xffffffffUL - next) + prev + 1u);
      signed_diff = -1 * signed_diff;
    }
  } else{
    const unsigned long diff = prev - next;
    if(diff <= half_max_unsigned_long){
      signed_diff = static_cast<long>(diff);
      signed_diff = -1 * signed_diff;
    }else{
      signed_diff = static_cast<long>((0xffffffffUL - prev) + next + 1u);
    }
  } 
  return signed_diff;
}


long TimePassedSince(unsigned long timeStamp){
  return TimeDifference(timeStamp, millis());
}

bool TimeReached(unsigned long timer){
  const long pass = TimePassedSince(timer);
  return (pass >= 0);
}

void SetNextTimeInterval(unsigned long& timer, const unsigned long step){
  timer += step;
  const long passed = TimePassedSince(timer);
  if(passed < 0) {return;}
  if (static_cast<unsigned long>(passed) > step){
    timer = millis() + step;
    return;
  }
  timer = millis() + (step - passed);
}
